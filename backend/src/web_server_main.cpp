#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstring>
#include <map>  // ADD FOR SESSIONS
#include <random>  // ADD FOR SESSION IDS
#include "config.h"

// Include YOUR existing classes
#include "../include/System.h"
#include "../include/Station.h"
#include "../include/Destination.h"
#include "../include/Louage.h"
#include "../include/AccountManager.h"  // ADD FOR AUTH
#include "../include/session.h"  // ADD FOR SESSIONS
#include "html_templates.h"  // ADD FOR HTML GENERATION

using namespace std;

// GLOBAL STATE - MOVE FROM OLD MAIN
System louageSystem;
AccountManager accountManager;
map<string, Session> activeSessions;  // ADD FOR SESSIONS

// Helper function to escape HTML (KEEP FROM OLD)
string escapeHTML(const string& data) {
    string buffer;
    buffer.reserve(data.size());
    for(size_t pos = 0; pos != data.size(); ++pos) {
        switch(data[pos]) {
            case '&':  buffer.append("&amp;");  break;
            case '\"': buffer.append("&quot;"); break;
            case '\'': buffer.append("&apos;"); break;
            case '<':  buffer.append("&lt;");   break;
            case '>':  buffer.append("&gt;");   break;
            default:   buffer.append(&data[pos], 1); break;
        }
    }
    return buffer;
}

// ADD: Generate random session ID
string generateSessionId() {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dis(0, 15);
    const char* chars = "0123456789abcdef";
    string id;
    for (int i = 0; i < 32; i++) id += chars[dis(gen)];
    return id;
}

// ADD: Extract cookie from request
string getCookie(const string& request, const string& name) {
    string cookieHeader = "Cookie: ";
    size_t pos = request.find(cookieHeader);
    if (pos == string::npos) return "";
    
    pos += cookieHeader.length();
    size_t end = request.find("\r\n", pos);
    string cookies = request.substr(pos, end - pos);
    
    pos = cookies.find(name + "=");
    if (pos == string::npos) return "";
    
    pos += name.length() + 1;
    end = cookies.find(";", pos);
    return cookies.substr(pos, (end == string::npos ? cookies.length() : end) - pos);
}

// ADD: Get authenticated session
Session* getAuthenticatedSession(const string& request) {
    string sessionId = getCookie(request, "session_id");
    if (sessionId.empty()) return nullptr;
    
    auto it = activeSessions.find(sessionId);
    if (it == activeSessions.end()) return nullptr;
    
    // Update last activity
    it->second.lastActivity = time(nullptr);
    
    // Check expiry (e.g., 30 min inactivity)
    if (difftime(it->second.lastActivity, it->second.createdAt) > 1800) {
        activeSessions.erase(it);
        return nullptr;
    }
    
    return &it->second;
}

// ADD: URL decode
string urlDecode(const string& str) {
    string result;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%' && i + 2 < str.length()) {
            int hex = stoi(str.substr(i + 1, 2), nullptr, 16);
            result += static_cast<char>(hex);
            i += 2;
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}

// ADD: Parse POST data
map<string, string> parsePostData(const string& body) {
    map<string, string> data;
    stringstream ss(body);
    string pair;
    while (getline(ss, pair, '&')) {
        size_t eqPos = pair.find('=');
        if (eqPos != string::npos) {
            string key = urlDecode(pair.substr(0, eqPos));
            string value = urlDecode(pair.substr(eqPos + 1));
            data[key] = value;
        }
    }
    return data;
}

// ADD: Handle OPTIONS for CORS preflight
string handleOptions() {
    return "HTTP/1.1 200 OK\r\n"
           "Access-Control-Allow-Origin: *\r\n"
           "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
           "Access-Control-Allow-Headers: Content-Type\r\n"
           "Content-Length: 0\r\n\r\n";
}

string handleLogin(const string& request) {
    // Get body
    size_t contentPos = request.find("\r\n\r\n");
    if (contentPos == string::npos) {
        return "HTTP/1.1 400 Bad Request\r\n\r\n";
    }
    string body = request.substr(contentPos + 4);
    
    auto params = parsePostData(body);
    string username = params["username"];
    string password = params["password"];
    
    cout << "Login attempt: " << username << endl;  // ADD THIS FOR DEBUGGING
    
    User* user = accountManager.login(username, password);
    if (user) {
        cout << "Login successful for: " << username << endl;  // ADD THIS
        
        // Create session
        string sessionId = generateSessionId();
        Session session;
        session.username = username;
        session.role = user->getRole();
        session.createdAt = time(nullptr);
        session.lastActivity = session.createdAt;
        activeSessions[sessionId] = session;
        
        // Set cookie and redirect
        stringstream ss;
        ss << "HTTP/1.1 302 Found\r\n"
           << "Location: /dashboard\r\n"
           << "Set-Cookie: session_id=" << sessionId << "; Path=/; HttpOnly\r\n"
           << "\r\n";
        return ss.str();
    } else {
        cout << "Login failed for: " << username << endl;  // ADD THIS
        
        return "HTTP/1.1 302 Found\r\n"
               "Location: /login?error=Invalid%20credentials\r\n"
               "\r\n";
    }
}

// ADD: Handle Signup (POST)
string handleSignup(const string& request) {
    // Get body
    size_t contentPos = request.find("\r\n\r\n");
    if (contentPos == string::npos) {
        return "HTTP/1.1 400 Bad Request\r\n\r\n";
    }
    string body = request.substr(contentPos + 4);
    
    auto params = parsePostData(body);
    string role = params["role"];
    string username = params["username"];
    string email = params["email"];
    string phone = params["phone"];
    string password = params["password"];
    
    bool success = false;
    if (role == "passenger") {
        success = accountManager.registerPassenger(username, password, email, phone);
    } else if (role == "driver") {
        string license = params["license"];
        int experience = stoi(params["experience"]);
        success = accountManager.registerDriver(username, password, email, phone, license, experience);
    } else if (role == "cashier") {
        string station = params["station"];
        success = accountManager.registerCashier(username, password, email, phone, station);
    } else if (role == "manager") {
        success = accountManager.registerManager(username, password, email, phone);
    }
    
    if (success) {
        return "HTTP/1.1 302 Found\r\n"
               "Location: /login?success=Account%20created\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "\r\n";
    } else {
        return "HTTP/1.1 302 Found\r\n"
               "Location: /signup?error=Registration%20failed\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "\r\n";
    }
}

// ADD: Handle Logout
string handleLogout(const string& request) {
    string sessionId = getCookie(request, "session_id");
    if (!sessionId.empty()) {
        activeSessions.erase(sessionId);
    }
    
    return "HTTP/1.1 302 Found\r\n"
           "Location: /\r\n"
           "Set-Cookie: session_id=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n"
           "Access-Control-Allow-Origin: *\r\n"
           "\r\n";
}

// ADD: Handle Dashboard
string handleDashboard(const string& request) {
    Session* session = getAuthenticatedSession(request);
    if (!session) {
        return "HTTP/1.1 302 Found\r\n"
               "Location: /login\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "\r\n";
    }
    
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html; charset=UTF-8\r\n"
           "Access-Control-Allow-Origin: *\r\n"
           "\r\n" + getDashboardPage(session);
}

// ADD: Handle API Destinations
string handleAPIDestinations(const string& request) {
    size_t pos = request.find("station=");
    if (pos == string::npos) {
        return "HTTP/1.1 400 Bad Request\r\n\r\nMissing station parameter";
    }
    
    string station = urlDecode(request.substr(pos + 8));
    size_t end = station.find_first_of(" &");
    if (end != string::npos) station = station.substr(0, end);
    
    vector<string> destinations = louageSystem.getDestinationsFromStation(station);
    sort(destinations.begin(), destinations.end());
    
    stringstream json;
    json << "{\"destinations\":[";
    for (size_t i = 0; i < destinations.size(); ++i) {
        if (i > 0) json << ",";
        json << "\"" << escapeHTML(destinations[i]) << "\"";
    }
    json << "]}";
    
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: application/json; charset=UTF-8\r\n"
           "Access-Control-Allow-Origin: *\r\n"
           "\r\n" + json.str();
}

// ADD: Handle API Search
string handleAPISearch(const string& request) {
    size_t fromPos = request.find("from=");
    size_t toPos = request.find("&to=");
    if (fromPos == string::npos || toPos == string::npos) {
        return "HTTP/1.1 400 Bad Request\r\n\r\nMissing parameters";
    }
    
    string from = urlDecode(request.substr(fromPos + 5, toPos - fromPos - 5));
    string to = urlDecode(request.substr(toPos + 4));
    size_t end = to.find_first_of(" &");
    if (end != string::npos) to = to.substr(0, end);
    
    vector<Louage> louages = louageSystem.searchLouages(from, to);
    
    stringstream json;
    json << "{\"louages\":[";
    for (size_t i = 0; i < louages.size(); ++i) {
        if (i > 0) json << ",";
        const Louage& l = louages[i];
        json << "{"
             << "\"numero\":" << l.getnumero_louage() << ","
             << "\"serie\":" << l.getSerieVehicule() << ","
             << "\"proprietaire\":" << l.getIdProp() << ","
             << "\"depart\":\"" << escapeHTML(l.getDepart()) << "\","
             << "\"destination\":\"" << escapeHTML(l.getDestination()) << "\","
             << "\"placesOccupees\":" << l.getOccupiedSeats()
             << "}";
    }
    json << "]}";
    
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: application/json; charset=UTF-8\r\n"
           "Access-Control-Allow-Origin: *\r\n"
           "\r\n" + json.str();
}
string handleAPIReserve(const string& request) {
    Session* session = getAuthenticatedSession(request);
    if (!session) {
        return "HTTP/1.1 401 Unauthorized\r\n"
               "Content-Type: application/json\r\n"
               "\r\n{\"success\":false,\"error\":\"Not authenticated\"}";
    }
    
    // Get POST body
    size_t contentPos = request.find("\r\n\r\n");
    if (contentPos == string::npos) {
        return "HTTP/1.1 400 Bad Request\r\n\r\n";
    }
    string body = request.substr(contentPos + 4);
    auto params = parsePostData(body);
    
    string from = params["from"];
    string to = params["to"];
    string name = params["name"];
    
    // Find the station and make reservation
    Station* station = louageSystem.findStation(from);
    if (!station) {
        return "HTTP/1.1 200 OK\r\n"
               "Content-Type: application/json\r\n"
               "\r\n{\"success\":false,\"error\":\"Station not found\"}";
    }
    
    int reservationNumber = station->makeReservation(to, name, true);
    
    if (reservationNumber > 0) {
        stringstream json;
        json << "{\"success\":true,\"reservationNumber\":" << reservationNumber << "}";
        return "HTTP/1.1 200 OK\r\n"
               "Content-Type: application/json\r\n"
               "\r\n" + json.str();
    } else {
        return "HTTP/1.1 200 OK\r\n"
               "Content-Type: application/json\r\n"
               "\r\n{\"success\":false,\"error\":\"Failed to create reservation\"}";
    }
}

string handleReservationPage(const string& request) {
    Session* session = getAuthenticatedSession(request);
    if (!session) {
        return "HTTP/1.1 302 Found\r\n"
               "Location: /login\r\n"
               "\r\n";
    }
    
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html; charset=UTF-8\r\n"
           "\r\n" + getReservationPage(session, louageSystem);
}

string handleAPIMyReservations(const string& request) {
    Session* session = getAuthenticatedSession(request);
    if (!session) {
        return "HTTP/1.1 401 Unauthorized\r\n\r\n{\"success\":false}";
    }
    
    // Find all reservations for this user across all destinations
    stringstream json;
    json << "{\"reservations\":[";
    
    bool first = true;
    vector<string> stationNames = louageSystem.getAllStationNames();
    
    for (const auto& stationName : stationNames) {
        Station* station = louageSystem.findStation(stationName);
        if (!station) continue;
        
        for (auto* dest : station->getDestinations()) {
            const ReservationSystem* rs = dest->getReservationSystem();
            if (!rs || !rs->getQueue()) continue;
            
            vector<Reservation*> allRes = rs->getQueue()->getAllReservations();
            
            for (Reservation* r : allRes) {
                if (r->getPassengerName() == session->username) {
                    if (!first) json << ",";
                    first = false;
                    
                    json << "{"
                         << "\"number\":" << r->getNumber() << ","
                         << "\"name\":\"" << escapeHTML(r->getPassengerName()) << "\","
                         << "\"from\":\"" << escapeHTML(stationName) << "\","
                         << "\"to\":\"" << escapeHTML(r->getDestination()) << "\","
                         << "\"paid\":" << (r->isPaid() ? "true" : "false") << ","
                         << "\"timesCalled\":" << r->gettimescalled()
                         << "}";
                }
            }
        }
    }
    
    json << "]}";
    
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: application/json\r\n"
           "\r\n" + json.str();
}

// Handle Pay Reservation API
string handleAPIPayReservation(const string& request) {
    Session* session = getAuthenticatedSession(request);
    if (!session) {
        return "HTTP/1.1 401 Unauthorized\r\n\r\n{\"success\":false}";
    }
    
    size_t contentPos = request.find("\r\n\r\n");
    if (contentPos == string::npos) {
        return "HTTP/1.1 400 Bad Request\r\n\r\n{\"success\":false}";
    }
    
    string body = request.substr(contentPos + 4);
    auto params = parsePostData(body);
    int resNumber = stoi(params["number"]);
    
    // Find and pay the reservation
    vector<string> stationNames = louageSystem.getAllStationNames();
    
    for (const auto& stationName : stationNames) {
        Station* station = louageSystem.findStation(stationName);
        if (!station) continue;
        
        for (auto* dest : station->getDestinations()) {
            if (dest->confirmerPaiement(resNumber)) {
                return "HTTP/1.1 200 OK\r\n"
                       "Content-Type: application/json\r\n"
                       "\r\n{\"success\":true}";
            }
        }
    }
    
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: application/json\r\n"
           "\r\n{\"success\":false,\"error\":\"Reservation not found\"}";
}

// Handle Cancel Reservation API
string handleAPICancelReservation(const string& request) {
    Session* session = getAuthenticatedSession(request);
    if (!session) {
        return "HTTP/1.1 401 Unauthorized\r\n\r\n{\"success\":false}";
    }
    
    size_t contentPos = request.find("\r\n\r\n");
    if (contentPos == string::npos) {
        return "HTTP/1.1 400 Bad Request\r\n\r\n{\"success\":false}";
    }
    
    string body = request.substr(contentPos + 4);
    auto params = parsePostData(body);
    int resNumber = stoi(params["number"]);
    
    // Find and remove the reservation
    vector<string> stationNames = louageSystem.getAllStationNames();
    
    for (const auto& stationName : stationNames) {
        Station* station = louageSystem.findStation(stationName);
        if (!station) continue;
        
        for (auto* dest : station->getDestinations()) {
            ReservationSystem* rs = dest->getReservationSystem();
            if (!rs || !rs->getQueue()) continue;
            
            if (rs->getQueue()->removeReservation(resNumber)) {
                return "HTTP/1.1 200 OK\r\n"
                       "Content-Type: application/json\r\n"
                       "\r\n{\"success\":true}";
            }
        }
    }
    
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: application/json\r\n"
           "\r\n{\"success\":false,\"error\":\"Reservation not found\"}";
}

// Handle My Profile API
string handleAPIMyProfile(const string& request) {
    Session* session = getAuthenticatedSession(request);
    if (!session) {
        return "HTTP/1.1 401 Unauthorized\r\n\r\n{\"success\":false}";
    }
    
    User* user = accountManager.findUserByUsername(session->username);
    if (!user) {
        return "HTTP/1.1 404 Not Found\r\n\r\n{\"success\":false}";
    }
    
    // Count reservations
    int totalRes = 0, activeRes = 0;
    vector<string> stationNames = louageSystem.getAllStationNames();
    
    for (const auto& stationName : stationNames) {
        Station* station = louageSystem.findStation(stationName);
        if (!station) continue;
        
        for (auto* dest : station->getDestinations()) {
            const ReservationSystem* rs = dest->getReservationSystem();
            if (!rs || !rs->getQueue()) continue;
            
            vector<Reservation*> allRes = rs->getQueue()->getAllReservations();
            for (Reservation* r : allRes) {
                if (r->getPassengerName() == session->username) {
                    totalRes++;
                    if (!r->isPaid()) activeRes++;
                }
            }
        }
    }
    
    stringstream json;
    json << "{"
         << "\"success\":true,"
         << "\"userId\":" << user->getUserID() << ","
         << "\"username\":\"" << escapeHTML(session->username) << "\","
         << "\"role\":\"" << escapeHTML(session->role) << "\","
         << "\"totalReservations\":" << totalRes << ","
         << "\"activeReservations\":" << activeRes << ","
         << "\"completedTrips\":0"
         << "}";
    
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: application/json\r\n"
           "\r\n" + json.str();
}

// Handle My Reservations Page
string handleMyReservationsPage(const string& request) {
    Session* session = getAuthenticatedSession(request);
    if (!session) {
        return "HTTP/1.1 302 Found\r\nLocation: /login\r\n\r\n";
    }
    
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html; charset=UTF-8\r\n"
           "\r\n" + getMyReservationsPage(session, louageSystem);
}

// Handle Profile Page
string handleProfilePage(const string& request) {
    Session* session = getAuthenticatedSession(request);
    if (!session) {
        return "HTTP/1.1 302 Found\r\nLocation: /login\r\n\r\n";
    }
    
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html; charset=UTF-8\r\n"
           "\r\n" + getPassengerProfilePage(session, accountManager);
}

int main() {
    louageSystem.loadFromFile("data/system_data.txt");
    accountManager.loadFromFile("data/accounts.txt");
    
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Erreur lors de l'initialisation de Winsock." << endl;
        return 1;
    }
    
    SOCKET_TYPE serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Erreur lors de la creation du socket." << endl;
        WSACleanup();
        return 1;
    }
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);
    
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Erreur lors du bind." << endl;
        CLOSE_SOCKET(serverSocket);
        WSACleanup();
        return 1;
    }
    
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Erreur lors du listen." << endl;
        CLOSE_SOCKET(serverSocket);
        WSACleanup();
        return 1;
    }
    
    cout << "Serveur demarre sur le port 8080..." << endl;
    
    while (true) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET_TYPE clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Erreur lors de l'accept." << endl;
            continue;
        }
        
        char buffer[4096] = {0};
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            string request(buffer, bytesReceived);
            string response;
            
            if (request.find("OPTIONS ") == 0) {
                response = handleOptions();
            }
            else if (request.find("GET / ") != string::npos || request.find("GET /?") != string::npos) {
                Session* session = getAuthenticatedSession(request);
                response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html; charset=UTF-8\r\n"
                           "Access-Control-Allow-Origin: *\r\n"
                           "\r\n" + getHomePage(louageSystem, session);
            }
            else if (request.find("GET /login") != string::npos) {
                size_t errorPos = request.find("error=");
                string error = "";
                if (errorPos != string::npos) {
                    error = urlDecode(request.substr(errorPos + 6));
                }
                response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html; charset=UTF-8\r\n"
                           "Access-Control-Allow-Origin: *\r\n"
                           "\r\n" + getLoginPage(error);
            }
            else if (request.find("GET /signup") != string::npos) {
                size_t errorPos = request.find("error=");
                string error = "";
                if (errorPos != string::npos) {
                    error = urlDecode(request.substr(errorPos + 6));
                }
                response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html; charset=UTF-8\r\n"
                           "Access-Control-Allow-Origin: *\r\n"
                           "\r\n" + getSignupPage(error);
            }
            else if (request.find("POST /login") != string::npos) {
                response = handleLogin(request);
            }
            else if (request.find("POST /signup") != string::npos) {
                response = handleSignup(request);
            }
            else if (request.find("GET /logout") != string::npos) {
                response = handleLogout(request);
            }
            else if (request.find("GET /dashboard") != string::npos) {
                response = handleDashboard(request);
            }
            else if (request.find("/api/destinations") != string::npos) {
                response = handleAPIDestinations(request);
            }
            else if (request.find("/api/search") != string::npos) {
                response = handleAPISearch(request);
            }
            else if (request.find("/favicon.ico") != string::npos) {
                response = 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: image/x-icon\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n";
            }
            else if (request.find("GET /reservation") != string::npos) {
                response = handleReservationPage(request);
            }
            else if (request.find("POST /api/reserve") != string::npos) {
                response = handleAPIReserve(request);
            }

            else if (request.find("GET /my-reservations") != string::npos) {
                response = handleMyReservationsPage(request);
            }
            // PROFILE PAGE
            else if (request.find("GET /profile") != string::npos) {
                response = handleProfilePage(request);
            }
            // API MY RESERVATIONS
            else if (request.find("GET /api/my-reservations") != string::npos) {
                response = handleAPIMyReservations(request);
            }
            // API PAY RESERVATION
            else if (request.find("POST /api/pay-reservation") != string::npos) {
                response = handleAPIPayReservation(request);
            }
            // API CANCEL RESERVATION
            else if (request.find("POST /api/cancel-reservation") != string::npos) {
                response = handleAPICancelReservation(request);
            }
            // API MY PROFILE
            else if (request.find("GET /api/my-profile") != string::npos) {
                response = handleAPIMyProfile(request);
            }
            else {
                response = 
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/html; charset=UTF-8\r\n"
                    "Access-Control-Allow-Origin: *\r\n"
                    "\r\n"
                    "<!DOCTYPE html>"
                    "<html><head><title>404</title>"
                    "<style>body{font-family:Arial;text-align:center;padding:50px;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:white;}</style></head>"
                    "<body>"
                    "<h1>404 - Page non trouvee</h1>"
                    "<a href='/' style='color:white;text-decoration:none;padding:10px 20px;background:rgba(255,255,255,0.2);border-radius:5px;'>Retour</a>"
                    "</body></html>";
            }
            
            // Send response
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        
        CLOSE_SOCKET(clientSocket);
    }
    
    CLOSE_SOCKET(serverSocket);
    WSACleanup();
    return 0;
}