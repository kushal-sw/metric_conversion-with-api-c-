#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <limits>
#include <string>
#include <curl/curl.h>
using namespace std;

// ============================================================
//                    TEMPERATURE CLASS
// ============================================================
class Temperature {
private:
    double value;
    string unit;

public:
    Temperature(double val, string u) : value(val), unit(u) {}

    double getValue() const { return value; }
    string getUnit()  const { return unit;  }

    double toCelsius() const {
        if (unit == "C") return value;
        if (unit == "F") return (value - 32) * 5.0 / 9.0;
        if (unit == "K") return value - 273.15;
        return 0;
    }

    double toFahrenheit() const {
        double c = toCelsius();
        return (c * 9.0 / 5.0) + 32;
    }

    double toKelvin() const {
        return toCelsius() + 273.15;
    }

    double convert(const string& toUnit) const {
        if (toUnit == "C") return toCelsius();
        if (toUnit == "F") return toFahrenheit();
        if (toUnit == "K") return toKelvin();
        return 0;
    }

    string classify() const {
        double c = toCelsius();
        if      (c <= 0)    return "Freezing";
        else if (c <= 15)   return "Cold";
        else if (c <= 25)   return "Comfortable";
        else if (c <= 37)   return "Warm";
        else if (c <= 100)  return "Hot";
        else                return "Boiling / Extreme Heat";
    }

    void displayAll() const {
        cout << "\n+------------------------------------------+" << endl;
        cout << "|           CONVERSION RESULTS             |" << endl;
        cout << "+------------------------------------------+" << endl;
        cout << fixed << setprecision(2);
        cout << "| Input       : " << setw(8) << value << " " << unit
             << "                     |" << endl;
        cout << "+------------------------------------------+" << endl;
        cout << "| Celsius     : " << setw(8) << toCelsius()
             << " \xB0" "C                  |" << endl;
        cout << "| Fahrenheit  : " << setw(8) << toFahrenheit()
             << " \xB0" "F                  |" << endl;
        cout << "| Kelvin      : " << setw(8) << toKelvin()
             << " K                   |" << endl;
        cout << "+------------------------------------------+" << endl;
        cout << "| Status      : " << classify()
             << string(26 - classify().length(), ' ') << "|" << endl;
        cout << "+------------------------------------------+" << endl;
    }
};

// ============================================================
//                    HISTORY MANAGER
// ============================================================
class HistoryManager {
private:
    struct Record {
        double inputValue;
        string inputUnit;
        double celsius;
        double fahrenheit;
        double kelvin;
        string status;
        string cityName; // empty if not from API
    };

    vector<Record> history;

public:
    void addRecord(const Temperature& t, const string& city = "") {
        Record r;
        r.inputValue  = t.getValue();
        r.inputUnit   = t.getUnit();
        r.celsius     = t.toCelsius();
        r.fahrenheit  = t.toFahrenheit();
        r.kelvin      = t.toKelvin();
        r.status      = t.classify();
        r.cityName    = city;
        history.push_back(r);
    }

    void displayHistory() const {
        if (history.empty()) {
            cout << "\nNo conversion history yet." << endl;
            return;
        }

        cout << "\n+======================================================+" << endl;
        cout << "|                 CONVERSION HISTORY                  |" << endl;
        cout << "+======================================================+" << endl;
        cout << fixed << setprecision(2);

        for (int i = 0; i < (int)history.size(); i++) {
            const Record& r = history[i];
            cout << "| #" << i + 1;
            if (!r.cityName.empty()) {
                cout << " [City: " << r.cityName << "]";
                cout << string(40 - r.cityName.length(), ' ') << "|" << endl;
            } else {
                cout << " | Input : " << setw(7) << r.inputValue
                     << " " << r.inputUnit
                     << "                              |" << endl;
            }
            cout << "|     | C: "  << setw(7) << r.celsius
                 << "  F: " << setw(7) << r.fahrenheit
                 << "  K: " << setw(7) << r.kelvin << " |" << endl;
            cout << "|     | Status: " << r.status
                 << string(38 - r.status.length(), ' ') << "|" << endl;
            cout << "+------------------------------------------------------+" << endl;
        }
    }

    void exportToFile() const {
        if (history.empty()) {
            cout << "\nNothing to export yet." << endl;
            return;
        }

        ofstream file("conversion_history.txt");
        if (!file) {
            cout << "\nError: Could not create file." << endl;
            return;
        }

        file << "==========================================\n";
        file << "     TEMPERATURE CONVERSION HISTORY\n";
        file << "==========================================\n\n";

        for (int i = 0; i < (int)history.size(); i++) {
            const Record& r = history[i];
            file << "Conversion #" << i + 1 << "\n";
            if (!r.cityName.empty())
                file << "City        : " << r.cityName << "\n";
            file << "Input       : " << r.inputValue << " " << r.inputUnit << "\n";
            file << "Celsius     : " << fixed << setprecision(2) << r.celsius    << " C\n";
            file << "Fahrenheit  : " << r.fahrenheit << " F\n";
            file << "Kelvin      : " << r.kelvin     << " K\n";
            file << "Status      : " << r.status     << "\n";
            file << "------------------------------------------\n";
        }

        file.close();
        cout << "\nHistory exported to 'conversion_history.txt' successfully!" << endl;
    }

    bool isEmpty() const { return history.empty(); }
};

// ============================================================
//                    API HELPER FUNCTIONS
// ============================================================

// Callback for curl to write HTTP response into a string
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Fetch live temperature of any city using OpenWeatherMap API
double getCityTemperature(const string& city, string& weatherDesc, string& country) {
    CURL* curl;
    CURLcode res;
    string response;

    string apiKey = "be0a7dd25b8068bf26b655cfcb1a606a";
    string url    = "https://api.openweathermap.org/data/2.5/weather?q="
                    + city + "&appid=" + apiKey + "&units=metric";

    curl = curl_easy_init();
    if (!curl) {
        cout << "Failed to initialize CURL." << endl;
        return -999;
    }

    curl_easy_setopt(curl, CURLOPT_URL,           url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,     &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,       10L); // 10 second timeout

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        cout << "\nError: Could not connect to weather service." << endl;
        cout << "Check your internet connection and try again." << endl;
        return -999;
    }

    // Check for API error response
    if (response.find("\"cod\":\"404\"") != string::npos) {
        cout << "\nError: City '" << city << "' not found. Check spelling and try again." << endl;
        return -999;
    }

    if (response.find("\"cod\":401") != string::npos) {
        cout << "\nError: Invalid API key. Please check your OpenWeatherMap API key." << endl;
        return -999;
    }

    // Parse temperature from JSON response
    double temp = -999;
    size_t tempPos = response.find("\"temp\":");
    if (tempPos != string::npos) {
        temp = stod(response.substr(tempPos + 7, 10));
    }

    // Parse weather description
    size_t descPos = response.find("\"description\":\"");
    if (descPos != string::npos) {
        size_t start = descPos + 15;
        size_t end   = response.find("\"", start);
        weatherDesc  = response.substr(start, end - start);
        // Capitalize first letter
        if (!weatherDesc.empty())
            weatherDesc[0] = toupper(weatherDesc[0]);
    }

    // Parse country code
    size_t countryPos = response.find("\"country\":\"");
    if (countryPos != string::npos) {
        size_t start = countryPos + 11;
        size_t end   = response.find("\"", start);
        country      = response.substr(start, end - start);
    }

    return temp;
}

// ============================================================
//                    INPUT VALIDATOR
// ============================================================
double getValidTemperature(const string& unit) {
    double temp;
    while (true) {
        cout << "Enter temperature in " << unit << ": ";
        cin >> temp;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a numeric value.\n";
            continue;
        }

        // Absolute zero check
        double celsius = temp;
        if (unit == "Fahrenheit") celsius = (temp - 32) * 5.0 / 9.0;
        if (unit == "Kelvin")     celsius = temp - 273.15;

        if (celsius < -273.15) {
            cout << "Physically impossible! Temperature cannot go below absolute zero (-273.15 C).\n";
            continue;
        }

        break;
    }
    return temp;
}

// ============================================================
//                    DISPLAY MENU
// ============================================================
void displayMenu() {
    cout << "\n+------------------------------------------+" << endl;
    cout << "|      TEMPERATURE CONVERSION SYSTEM       |" << endl;
    cout << "+------------------------------------------+" << endl;
    cout << "|  MANUAL CONVERSIONS                      |" << endl;
    cout << "|  1. Convert from Celsius                 |" << endl;
    cout << "|  2. Convert from Fahrenheit              |" << endl;
    cout << "|  3. Convert from Kelvin                  |" << endl;
    cout << "+------------------------------------------+" << endl;
    cout << "|  LIVE WEATHER                            |" << endl;
    cout << "|  7. Get City Temperature & Convert       |" << endl;
    cout << "+------------------------------------------+" << endl;
    cout << "|  HISTORY                                 |" << endl;
    cout << "|  4. View Conversion History              |" << endl;
    cout << "|  5. Export History to File               |" << endl;
    cout << "+------------------------------------------+" << endl;
    cout << "|  6. Exit                                 |" << endl;
    cout << "+------------------------------------------+" << endl;
    cout << "Your choice: ";
}

// ============================================================
//                        MAIN
// ============================================================
int main() {
    HistoryManager history;
    int choice;

    cout << "\n  Welcome to the Advanced Temperature Conversion System!" << endl;
    cout << "  Powered by OpenWeatherMap API\n" << endl;

    do {
        displayMenu();
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number between 1 and 7." << endl;
            continue;
        }

        // ── Manual Conversions (1, 2, 3) ─────────────────────
        if (choice >= 1 && choice <= 3) {
            string units[]     = {"C",        "F",           "K"      };
            string unitNames[] = {"Celsius",  "Fahrenheit",  "Kelvin" };
            string selectedUnit = units[choice - 1];

            double temp = getValidTemperature(unitNames[choice - 1]);

            Temperature t(temp, selectedUnit);
            t.displayAll();
            history.addRecord(t);

            char again;
            cout << "\nConvert another with same unit? (y/n): ";
            cin >> again;
            while (tolower(again) == 'y') {
                temp = getValidTemperature(unitNames[choice - 1]);
                Temperature t2(temp, selectedUnit);
                t2.displayAll();
                history.addRecord(t2);
                cout << "\nConvert another with same unit? (y/n): ";
                cin >> again;
            }
        }

        // ── View History ──────────────────────────────────────
        else if (choice == 4) {
            history.displayHistory();
        }

        // ── Export to File ────────────────────────────────────
        else if (choice == 5) {
            history.exportToFile();
        }

        // ── Exit ──────────────────────────────────────────────
        else if (choice == 6) {
            if (!history.isEmpty()) {
                char save;
                cout << "\nExport history before exiting? (y/n): ";
                cin >> save;
                if (tolower(save) == 'y') {
                    history.exportToFile();
                }
            }
            cout << "\nThank you for using the Temperature Conversion System. Goodbye!\n" << endl;
        }

        // ── City Temperature via API ──────────────────────────
        else if (choice == 7) {
            string city;
            cout << "\nEnter city name: ";
            cin.ignore();
            getline(cin, city);

            cout << "\nFetching live weather data for '" << city << "'..." << endl;

            string weatherDesc = "Unknown";
            string country     = "";
            double cityTemp    = getCityTemperature(city, weatherDesc, country);

            if (cityTemp == -999) {
                // Error already printed inside getCityTemperature()
                continue;
            }

            // Display weather info
            cout << "\n+------------------------------------------+" << endl;
            cout << "|            LIVE WEATHER DATA             |" << endl;
            cout << "+------------------------------------------+" << endl;
            cout << "| City        : " << city;
            if (!country.empty()) cout << ", " << country;
            cout << string(max(0, (int)(26 - city.length() - country.length() - 2)), ' ')
                 << "|" << endl;
            cout << "| Condition   : " << weatherDesc
                 << string(26 - weatherDesc.length(), ' ') << "|" << endl;
            cout << "| Live Temp   : " << fixed << setprecision(2)
                 << cityTemp << " \xB0" "C"
                 << "                   |" << endl;
            cout << "+------------------------------------------+" << endl;

            // Feed into Temperature class and show all conversions
            Temperature t(cityTemp, "C");
            t.displayAll();
            history.addRecord(t, city);
        }

        else {
            cout << "Invalid choice! Please select between 1 and 7." << endl;
        }

    } while (choice != 6);

    return 0;
}