#include "html_templates.h"
#include "../include/AccountManager.h"
#include <sstream>
#include <algorithm>

using namespace std;

extern string escapeHTML(const string& data);

// ============= COMMON CSS =============
string getCommonCSS() {
    return R"xxx(
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: #333;
            line-height: 1.6;
            min-height: 100vh;
        }
        
        .navbar {
            background: rgba(255, 255, 255, 0.95);
            padding: 15px 30px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        
        .navbar-brand {
            font-size: 1.5rem;
            font-weight: bold;
            color: #667eea;
            text-decoration: none;
            display: flex;
            align-items: center;
            gap: 10px;
        }
        
        .navbar-menu {
            display: flex;
            gap: 20px;
            align-items: center;
        }
        
        .navbar-menu a {
            color: #333;
            text-decoration: none;
            padding: 8px 15px;
            border-radius: 5px;
            transition: all 0.3s;
        }
        
        .navbar-menu a:hover {
            background: #667eea;
            color: white;
        }
        
        .btn-logout {
            background: #dc3545 !important;
            color: white !important;
        }
        
        .container {
            max-width: 1200px;
            margin: 40px auto;
            padding: 0 20px;
        }
        
        .card {
            background: white;
            border-radius: 15px;
            padding: 40px;
            box-shadow: 0 20px 40px rgba(0,0,0,0.1);
            margin-bottom: 20px;
        }
        
        .form-group {
            margin-bottom: 20px;
        }
        
        .form-group label {
            display: block;
            margin-bottom: 8px;
            font-weight: 600;
            color: #555;
        }
        
        .form-control {
            width: 100%;
            padding: 12px 15px;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            font-size: 16px;
            transition: border-color 0.3s;
        }
        
        .form-control:focus {
            border-color: #667eea;
            outline: none;
        }
        
        .btn {
            padding: 12px 30px;
            background: linear-gradient(135deg, #667eea, #764ba2);
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s;
            display: inline-block;
            text-decoration: none;
        }
        
        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);
        }
        
        .alert {
            padding: 15px 20px;
            border-radius: 8px;
            margin-bottom: 20px;
        }
        
        .alert-success {
            background: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        
        .alert-error {
            background: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
        
        .text-center {
            text-align: center;
        }
        
        .mt-3 {
            margin-top: 20px;
        }
        
        @media (max-width: 768px) {
            .navbar {
                flex-direction: column;
                gap: 15px;
            }
            
            .navbar-menu {
                flex-direction: column;
                width: 100%;
            }
        }
    )xxx";
}

// ============= NAVBAR =============
string getNavbar(Session* session) {
    stringstream html;
    html << R"xxx(
    <nav class="navbar">
        <a href="/" class="navbar-brand">
            <i class="fas fa-bus"></i> Louage Tracker
        </a>
        <div class="navbar-menu">
)xxx";
    
    if (session) {
        html << "            <a href=\"/dashboard\"><i class=\"fas fa-tachometer-alt\"></i> Dashboard</a>\n";
        html << "            <span>Bonjour, " << escapeHTML(session->username) << "</span>\n";
        html << "            <a href=\"/logout\" class=\"btn-logout\"><i class=\"fas fa-sign-out-alt\"></i> Déconnexion</a>\n";
    } else {
        html << "            <a href=\"/login\"><i class=\"fas fa-sign-in-alt\"></i> Connexion</a>\n";
        html << "            <a href=\"/signup\"><i class=\"fas fa-user-plus\"></i> Inscription</a>\n";
    }
    
    html << R"xxx(        </div>
    </nav>
)xxx";
    
    return html.str();
}

// ============= LOGIN PAGE =============
string getLoginPage(const string& error) {
    stringstream html;
    html << R"xxx(<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Connexion - Louage Tracker</title>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <style>)xxx" << getCommonCSS() << R"xxx(
        .login-container {
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            padding: 20px;
        }
        
        .login-card {
            background: white;
            border-radius: 15px;
            padding: 40px;
            box-shadow: 0 20px 40px rgba(0,0,0,0.1);
            max-width: 450px;
            width: 100%;
        }
        
        .login-header {
            text-align: center;
            margin-bottom: 30px;
        }
        
        .login-header i {
            font-size: 3rem;
            color: #667eea;
            margin-bottom: 15px;
        }
        
        .login-header h2 {
            color: #333;
            margin-bottom: 10px;
        }
    </style>
</head>
<body>
    <div class="login-container">
        <div class="login-card">
            <div class="login-header">
                <i class="fas fa-bus"></i>
                <h2>Louage Tracker</h2>
                <p>Connectez-vous à votre compte</p>
            </div>
)xxx";
    
    if (!error.empty()) {
        html << "            <div class=\"alert alert-error\"><i class=\"fas fa-exclamation-circle\"></i> " 
             << escapeHTML(error) << "</div>\n";
    }
    
    html << R"xxx(            
            <form method="POST" action="/login">
                <div class="form-group">
                    <label for="username"><i class="fas fa-user"></i> Nom d'utilisateur</label>
                    <input type="text" id="username" name="username" class="form-control" required autofocus>
                </div>
                
                <div class="form-group">
                    <label for="password"><i class="fas fa-lock"></i> Mot de passe</label>
                    <input type="password" id="password" name="password" class="form-control" required>
                </div>
                
                <button type="submit" class="btn" style="width: 100%;">
                    <i class="fas fa-sign-in-alt"></i> Se connecter
                </button>
            </form>
            
            <div class="text-center mt-3">
                <p>Pas encore de compte? <a href="/signup" style="color: #667eea; font-weight: bold;">S'inscrire</a></p>
                <p style="margin-top: 10px;"><a href="/" style="color: #667eea;"><i class="fas fa-home"></i> Retour à l'accueil</a></p>
            </div>
        </div>
    </div>
</body>
</html>)xxx";
    
    return html.str();
}

// ============= SIGNUP PAGE =============
string getSignupPage(const string& error) {
    stringstream html;
    html << R"xxx(<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Inscription - Louage Tracker</title>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <style>)xxx" << getCommonCSS() << R"xxx(
        .signup-container {
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            padding: 20px;
        }
        
        .signup-card {
            background: white;
            border-radius: 15px;
            padding: 40px;
            box-shadow: 0 20px 40px rgba(0,0,0,0.1);
            max-width: 600px;
            width: 100%;
        }
        
        .signup-header {
            text-align: center;
            margin-bottom: 30px;
        }
        
        .signup-header i {
            font-size: 3rem;
            color: #667eea;
            margin-bottom: 15px;
        }
    </style>
</head>
<body>
    <div class="signup-container">
        <div class="signup-card">
            <div class="signup-header">
                <i class="fas fa-user-plus"></i>
                <h2>Créer un compte</h2>
                <p>Inscrivez-vous pour accéder à toutes les fonctionnalités</p>
            </div>
)xxx";
    
    if (!error.empty()) {
        html << "            <div class=\"alert alert-error\"><i class=\"fas fa-exclamation-circle\"></i> " 
             << escapeHTML(error) << "</div>\n";
    }
    
    html << R"xxx(            
            <form method="POST" action="/signup">
                <div class="form-group">
                    <label for="role"><i class="fas fa-user-tag"></i> Type de compte</label>
                    <select id="role" name="role" class="form-control" required>
                        <option value="passenger">Passager</option>
                        <option value="driver">Chauffeur</option>
                        <option value="cashier">Caissier</option>
                        <option value="manager">Manager</option>
                    </select>
                </div>
                
                <div class="form-group">
                    <label for="username"><i class="fas fa-user"></i> Nom d'utilisateur</label>
                    <input type="text" id="username" name="username" class="form-control" required>
                </div>
                
                <div class="form-group">
                    <label for="email"><i class="fas fa-envelope"></i> Email</label>
                    <input type="email" id="email" name="email" class="form-control" required>
                </div>
                
                <div class="form-group">
                    <label for="phone"><i class="fas fa-phone"></i> Téléphone</label>
                    <input type="tel" id="phone" name="phone" class="form-control" required placeholder="Ex: 98765432">
                </div>
                
                <div class="form-group">
                    <label for="password"><i class="fas fa-lock"></i> Mot de passe</label>
                    <input type="password" id="password" name="password" class="form-control" required minlength="6">
                    <small style="color: #666;">Minimum 6 caractères</small>
                </div>
                
                <div id="driverFields" style="display:none;">
                    <div class="form-group">
                        <label for="license"><i class="fas fa-id-card"></i> Numéro de permis</label>
                        <input type="text" id="license" name="license" class="form-control">
                    </div>
                    <div class="form-group">
                        <label for="experience"><i class="fas fa-calendar"></i> Années d'expérience</label>
                        <input type="number" id="experience" name="experience" class="form-control" min="0" value="0">
                    </div>
                </div>
                
                <div id="cashierFields" style="display:none;">
                    <div class="form-group">
                        <label for="station"><i class="fas fa-building"></i> Station assignée</label>
                        <input type="text" id="station" name="station" class="form-control" placeholder="Ex: Tunis">
                    </div>
                </div>
                
                <button type="submit" class="btn" style="width: 100%;">
                    <i class="fas fa-user-plus"></i> S'inscrire
                </button>
            </form>
            
            <div class="text-center mt-3">
                <p>Déjà un compte? <a href="/login" style="color: #667eea; font-weight: bold;">Se connecter</a></p>
                <p style="margin-top: 10px;"><a href="/" style="color: #667eea;"><i class="fas fa-home"></i> Retour à l'accueil</a></p>
            </div>
        </div>
    </div>
    
    <script>
        document.getElementById('role').addEventListener('change', function() {
            const role = this.value;
            document.getElementById('driverFields').style.display = (role === 'driver') ? 'block' : 'none';
            document.getElementById('cashierFields').style.display = (role === 'cashier') ? 'block' : 'none';
            
            // Update required fields
            document.getElementById('license').required = (role === 'driver');
            document.getElementById('experience').required = (role === 'driver');
            document.getElementById('station').required = (role === 'cashier');
        });
    </script>
</body>
</html>)xxx";
    
    return html.str();
}

// ============= HOME PAGE (PUBLIC SEARCH) =============
// ============================================
// UPDATE getHomePage() IN html_templates.cpp
// REPLACE THE ENTIRE FUNCTION WITH THIS:
// ============================================

string getHomePage(System& system, Session* session) {
    stringstream html;
    html << "<!DOCTYPE html>\n<html lang=\"fr\">\n<head>\n";
    html << "<meta charset=\"UTF-8\">\n";
    html << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "<title>Louage Tracker Tunisie</title>\n";
    html << "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css\">\n";
    html << "<style>" << getCommonCSS();
    
    // Add extra styles
    html << ".hero { background: white; border-radius: 15px; padding: 40px; margin-bottom: 30px; text-align: center; box-shadow: 0 20px 40px rgba(0,0,0,0.1); }\n";
    html << ".hero h1 { color: #333; margin-bottom: 10px; display: flex; align-items: center; justify-content: center; gap: 15px; flex-wrap: wrap; }\n";
    html << ".hero h1 i { color: #667eea; }\n";
    html << ".search-form { display: grid; grid-template-columns: 1fr 1fr auto; gap: 15px; margin-top: 30px; align-items: end; }\n";
    html << "#searchResults { margin-top: 30px; }\n";
    html << ".result-card { background: white; border: 2px solid #e0e0e0; border-radius: 10px; padding: 20px; margin-bottom: 15px; transition: all 0.3s; }\n";
    html << ".result-card:hover { transform: translateY(-3px); box-shadow: 0 8px 20px rgba(102, 126, 234, 0.3); border-color: #667eea; }\n";
    html << ".result-card h4 { color: #667eea; margin-bottom: 10px; display: flex; align-items: center; gap: 10px; }\n";
    html << ".info-message { text-align: center; padding: 40px; color: #666; }\n";
    html << ".info-message i { font-size: 3rem; color: #667eea; margin-bottom: 15px; }\n";
    
    // NEW STYLES FOR RESERVATIONS SECTION
    html << ".reservations-section { background: white; border-radius: 15px; padding: 40px; margin-top: 30px; box-shadow: 0 20px 40px rgba(0,0,0,0.1); }\n";
    html << ".reservations-section h2 { color: #333; margin-bottom: 25px; display: flex; align-items: center; gap: 10px; }\n";
    html << ".station-card { background: #f8f9fa; border-radius: 10px; padding: 20px; margin-bottom: 20px; border-left: 4px solid #667eea; }\n";
    html << ".station-card h3 { color: #667eea; margin-bottom: 15px; }\n";
    html << ".destination-group { margin-bottom: 20px; padding: 15px; background: white; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.05); }\n";
    html << ".destination-group h4 { color: #333; margin-bottom: 10px; font-size: 1.1rem; }\n";
    html << ".reservation-item { padding: 10px; margin: 5px 0; background: #fff; border-left: 3px solid #28a745; border-radius: 5px; display: flex; justify-content: space-between; align-items: center; }\n";
    html << ".reservation-item.unpaid { border-left-color: #ffc107; }\n";
    html << ".reservation-number { font-weight: bold; color: #667eea; }\n";
    html << ".passenger-name { color: #333; }\n";
    html << ".status-badge { padding: 4px 12px; border-radius: 15px; font-size: 0.85rem; font-weight: bold; }\n";
    html << ".status-paid { background: #d4edda; color: #155724; }\n";
    html << ".status-waiting { background: #fff3cd; color: #856404; }\n";
    html << ".no-reservations { text-align: center; padding: 20px; color: #999; font-style: italic; }\n";
    html << ".stats-summary { display: flex; gap: 20px; margin-bottom: 20px; flex-wrap: wrap; }\n";
    html << ".stat-box { flex: 1; min-width: 150px; background: linear-gradient(135deg, #667eea, #764ba2); color: white; padding: 20px; border-radius: 10px; text-align: center; }\n";
    html << ".stat-box h3 { font-size: 2rem; margin-bottom: 5px; }\n";
    html << ".stat-box p { font-size: 0.9rem; opacity: 0.9; }\n";
    
    html << "@media (max-width: 768px) { .search-form { grid-template-columns: 1fr; } }\n";
    html << "</style>\n</head>\n<body>\n";
    
    html << getNavbar(session);
    
    html << "<div class=\"container\">\n";
    
    // SEARCH SECTION
    html << "<div class=\"hero\">\n";
    html << "<h1><i class=\"fas fa-bus\"></i> Louage Tracker Tunisie</h1>\n";
    html << "<p style=\"color: #666; font-size: 1.1rem;\">Recherchez des louages entre les stations tunisiennes</p>\n";
    html << "<form class=\"search-form\" onsubmit=\"return performSearch()\">\n";
    html << "<div class=\"form-group\">\n";
    html << "<label for=\"fromStation\"><i class=\"fas fa-map-pin\"></i> Station de départ</label>\n";
    html << "<select id=\"fromStation\" class=\"form-control\" required>\n";
    html << "<option value=\"\">-- Sélectionnez une station --</option>\n";
    
    vector<string> stations = system.getAllStationNames();
    sort(stations.begin(), stations.end());
    for (const auto& station : stations) {
        html << "<option value=\"" << escapeHTML(station) << "\">" << escapeHTML(station) << "</option>\n";
    }
    
    html << "</select>\n</div>\n";
    html << "<div class=\"form-group\">\n";
    html << "<label for=\"toDestination\"><i class=\"fas fa-flag-checkered\"></i> Destination</label>\n";
    html << "<select id=\"toDestination\" class=\"form-control\" required disabled>\n";
    html << "<option value=\"\">-- Sélectionnez d'abord le départ --</option>\n";
    html << "</select>\n</div>\n";
    html << "<div class=\"form-group\">\n";
    html << "<label>&nbsp;</label>\n";
    html << "<button type=\"submit\" class=\"btn\"><i class=\"fas fa-search\"></i> Rechercher</button>\n";
    html << "</div>\n</form>\n";
    html << "<div id=\"searchResults\">\n";
    html << "<div class=\"info-message\"><i class=\"fas fa-info-circle\"></i>\n";
    html << "<p>Sélectionnez une station et une destination pour rechercher des louages</p>\n";
    html << "</div>\n</div>\n</div>\n";
    
    // NEW: ALL RESERVATIONS SECTION
    html << "<div class=\"reservations-section\">\n";
    html << "<h2><i class=\"fas fa-clipboard-list\"></i> Toutes les Réservations</h2>\n";
    
    // Calculate statistics
    int totalReservations = 0;
    int paidReservations = 0;
    int waitingReservations = 0;
    
    for (const auto& stationName : stations) {
        Station* station = system.findStation(stationName);
        if (!station) continue;
        
        for (auto* dest : station->getDestinations()) {
            const ReservationSystem* rs = dest->getReservationSystem();
            if (!rs || !rs->getQueue()) continue;
            
            vector<Reservation*> allRes = rs->getQueue()->getAllReservations();
            totalReservations += allRes.size();
            
            for (Reservation* r : allRes) {
                if (r->isPaid()) paidReservations++;
                else waitingReservations++;
            }
        }
    }
    
    // Display statistics
    html << "<div class=\"stats-summary\">\n";
    html << "<div class=\"stat-box\"><h3>" << totalReservations << "</h3><p>Total Réservations</p></div>\n";
    html << "<div class=\"stat-box\"><h3>" << paidReservations << "</h3><p>Payées</p></div>\n";
    html << "<div class=\"stat-box\"><h3>" << waitingReservations << "</h3><p>En Attente</p></div>\n";
    html << "</div>\n";
    
    // Display reservations by station and destination
    for (const auto& stationName : stations) {
        Station* station = system.findStation(stationName);
        if (!station) continue;
        
        bool hasReservations = false;
        stringstream stationContent;
        
        for (auto* dest : station->getDestinations()) {
            const ReservationSystem* rs = dest->getReservationSystem();
            if (!rs || !rs->getQueue()) continue;
            
            vector<Reservation*> allRes = rs->getQueue()->getAllReservations();
            
            if (!allRes.empty()) {
                hasReservations = true;
                stationContent << "<div class=\"destination-group\">\n";
                stationContent << "<h4><i class=\"fas fa-arrow-right\"></i> " << escapeHTML(dest->getNom()) 
                              << " <span style=\"color: #999; font-size: 0.9rem;\">(" << allRes.size() << " réservation(s))</span></h4>\n";
                
                for (Reservation* r : allRes) {
                    string itemClass = r->isPaid() ? "reservation-item" : "reservation-item unpaid";
                    stationContent << "<div class=\"" << itemClass << "\">\n";
                    stationContent << "<div>\n";
                    stationContent << "<span class=\"reservation-number\">#" << r->getNumber() << "</span> - ";
                    stationContent << "<span class=\"passenger-name\">" << escapeHTML(r->getPassengerName()) << "</span>";
                    stationContent << " <small style=\"color: #666;\">(Appelé " << r->gettimescalled() << " fois)</small>\n";
                    stationContent << "</div>\n";
                    
                    if (r->isPaid()) {
                        stationContent << "<span class=\"status-badge status-paid\"><i class=\"fas fa-check-circle\"></i> Payé</span>\n";
                    } else {
                        stationContent << "<span class=\"status-badge status-waiting\"><i class=\"fas fa-clock\"></i> En attente</span>\n";
                    }
                    
                    stationContent << "</div>\n";
                }
                
                stationContent << "</div>\n";
            }
        }
        
        if (hasReservations) {
            html << "<div class=\"station-card\">\n";
            html << "<h3><i class=\"fas fa-building\"></i> Station: " << escapeHTML(stationName) << "</h3>\n";
            html << stationContent.str();
            html << "</div>\n";
        }
    }
    
    if (totalReservations == 0) {
        html << "<div class=\"no-reservations\">\n";
        html << "<i class=\"fas fa-inbox\" style=\"font-size: 3rem; color: #ccc; margin-bottom: 15px;\"></i>\n";
        html << "<p>Aucune réservation pour le moment</p>\n";
        html << "</div>\n";
    }
    
    html << "</div>\n"; // End reservations-section
    html << "</div>\n"; // End container
    
    // JavaScript for search functionality
    html << "<script>\n";
    html << "document.getElementById('fromStation').addEventListener('change', function() {\n";
    html << "const fromStation = this.value;\n";
    html << "const toSelect = document.getElementById('toDestination');\n";
    html << "if (!fromStation) {\n";
    html << "toSelect.innerHTML = '<option value=\"\">-- Sélectionnez d\\'abord le départ --</option>';\n";
    html << "toSelect.disabled = true; return;\n";
    html << "}\n";
    html << "toSelect.innerHTML = '<option value=\"\">-- Chargement... --</option>';\n";
    html << "toSelect.disabled = true;\n";
    html << "fetch('/api/destinations?station=' + encodeURIComponent(fromStation))\n";
    html << ".then(response => response.json())\n";
    html << ".then(data => {\n";
    html << "toSelect.innerHTML = '<option value=\"\">-- Sélectionnez une destination --</option>';\n";
    html << "if (data.destinations && data.destinations.length > 0) {\n";
    html << "const uniqueDests = [...new Set(data.destinations)].sort();\n";
    html << "uniqueDests.forEach(dest => {\n";
    html << "const option = document.createElement('option');\n";
    html << "option.value = dest; option.textContent = dest;\n";
    html << "toSelect.appendChild(option);\n";
    html << "});\n";
    html << "toSelect.disabled = false;\n";
    html << "}\n";
    html << "})\n";
    html << ".catch(error => { console.error('Error:', error); toSelect.innerHTML = '<option value=\"\">-- Erreur --</option>'; });\n";
    html << "});\n";
    
    html << "function performSearch() {\n";
    html << "const fromStation = document.getElementById('fromStation').value;\n";
    html << "const toDestination = document.getElementById('toDestination').value;\n";
    html << "if (!fromStation || !toDestination) { alert('Veuillez sélectionner une station et une destination'); return false; }\n";
    html << "document.getElementById('searchResults').innerHTML = '<div class=\"info-message\"><i class=\"fas fa-spinner fa-spin\"></i><p>Recherche en cours... ' + fromStation + ' → ' + toDestination + '</p></div>';\n";
    html << "fetch('/api/search?from=' + encodeURIComponent(fromStation) + '&to=' + encodeURIComponent(toDestination))\n";
    html << ".then(response => response.json())\n";
    html << ".then(data => {\n";
    html << "if (data.louages && data.louages.length > 0) {\n";
    html << "let html = '<h3 style=\"color: #333; margin-bottom: 20px;\"><i class=\"fas fa-check-circle\" style=\"color: #28a745;\"></i> ' + data.louages.length + ' Louage(s) trouvé(s)</h3>';\n";
    html << "data.louages.forEach(louage => {\n";
    html << "html += '<div class=\"result-card\">';\n";
    html << "html += '<h4><i class=\"fas fa-bus\"></i> Louage #' + louage.numero + '</h4>';\n";
    html << "html += '<p style=\"font-size: 1.1rem; margin: 10px 0;\"><strong>' + louage.depart + ' → ' + louage.destination + '</strong></p>';\n";
    html << "html += '<p style=\"color: #666;\"><i class=\"fas fa-info-circle\"></i> Série: ' + louage.serie + ' | Propriétaire: #' + louage.proprietaire + '</p>';\n";
    html << "html += '</div>';\n";
    html << "});\n";
    html << "document.getElementById('searchResults').innerHTML = html;\n";
    html << "} else {\n";
    html << "document.getElementById('searchResults').innerHTML = '<div class=\"info-message\"><i class=\"fas fa-exclamation-circle\" style=\"color: #ffc107;\"></i><p>Aucun louage trouvé pour ' + fromStation + ' → ' + toDestination + '</p></div>';\n";
    html << "}\n";
    html << "})\n";
    html << ".catch(error => { document.getElementById('searchResults').innerHTML = '<div class=\"info-message\"><i class=\"fas fa-times-circle\" style=\"color: #dc3545;\"></i><p>Erreur: ' + error.message + '</p></div>'; });\n";
    html << "return false;\n";
    html << "}\n";
    html << "</script>\n";
    html << "</body>\n</html>\n";
    
    return html.str();
}
// ============= DASHBOARD PAGE =============
string getDashboardPage(Session* session) {
    stringstream html;
    html << R"xxx(<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Dashboard - Louage Tracker</title>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <style>)xxx" << getCommonCSS() << R"xxx(
        .dashboard-header {
            background: white;
            border-radius: 15px;
            padding: 30px;
            margin-bottom: 30px;
            box-shadow: 0 5px 15px rgba(0,0,0,0.1);
        }
        
        .dashboard-header h1 {
            color: #333;
            margin-bottom: 10px;
        }
        
        .role-badge {
            display: inline-block;
            padding: 5px 15px;
            background: linear-gradient(135deg, #667eea, #764ba2);
            color: white;
            border-radius: 20px;
            font-size: 0.9rem;
            margin-top: 10px;
        }
        
        .dashboard-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            margin-top: 30px;
        }
        
        .stat-card {
            background: white;
            border-radius: 10px;
            padding: 25px;
            text-align: center;
            box-shadow: 0 5px 15px rgba(0,0,0,0.1);
            transition: all 0.3s;
        }
        
        .stat-card:hover {
            transform: translateY(-5px);
            box-shadow: 0 8px 20px rgba(102, 126, 234, 0.3);
        }
        
        .stat-card i {
            font-size: 3rem;
            color: #667eea;
            margin-bottom: 15px;
        }
        
        .stat-card h3 {
            font-size: 2rem;
            color: #333;
            margin-bottom: 10px;
        }
        
        .stat-card p {
            color: #666;
        }
        
        .action-buttons {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin-top: 30px;
        }
        
        .action-btn {
            padding: 20px;
            background: white;
            border: 2px solid #e0e0e0;
            border-radius: 10px;
            text-align: center;
            cursor: pointer;
            transition: all 0.3s;
            text-decoration: none;
            color: #333;
            display: block;
        }
        
        .action-btn:hover {
            border-color: #667eea;
            background: #f8f9fa;
            transform: translateY(-3px);
        }
        
        .action-btn i {
            font-size: 2rem;
            color: #667eea;
            margin-bottom: 10px;
        }
    </style>
</head>
<body>
)xxx";
    
    html << getNavbar(session);
    
    html << R"xxx(
    <div class="container">
        <div class="dashboard-header">
            <h1><i class="fas fa-tachometer-alt"></i> Dashboard</h1>
            <p>Bienvenue, <strong>)xxx" << escapeHTML(session->username) << R"xxx(</strong></p>
            <span class="role-badge">)xxx" << escapeHTML(session->role) << R"xxx(</span>
        </div>
        
        <div class="dashboard-grid">
)xxx";
    
    // Role-specific stats
    if (session->role == "passenger") {
        html << R"xxx(
            <div class="stat-card">
                <i class="fas fa-ticket-alt"></i>
                <h3>0</h3>
                <p>Réservations actives</p>
            </div>
            <div class="stat-card">
                <i class="fas fa-history"></i>
                <h3>0</h3>
                <p>Trajets effectués</p>
            </div>
            <div class="stat-card">
                <i class="fas fa-money-bill-wave"></i>
                <h3>0 TND</h3>
                <p>Total dépensé</p>
            </div>
        </div>
        
        <div class="card">
            <h2><i class="fas fa-tasks"></i> Actions disponibles</h2>
            <div class="action-buttons">
                <a href="/" class="action-btn">
                    <i class="fas fa-search"></i>
                    <p><strong>Rechercher un louage</strong></p>
                </a>
                <a href="/reservation" class="action-btn">
                    <i class="fas fa-plus-circle"></i>
                    <p><strong>Nouvelle réservation</strong></p>
                </a>
                <div class="action-btn" onclick="alert('Fonctionnalité en cours de développement')">
                    <i class="fas fa-list"></i>
                    <p><strong>Mes réservations</strong></p>
                </div>
            </div>
        </div>
)xxx";
    } else if (session->role == "driver") {
        html << R"xxx(
            <div class="stat-card">
                <i class="fas fa-bus"></i>
                <h3>N/A</h3>
                <p>Louage assigné</p>
            </div>
            <div class="stat-card">
                <i class="fas fa-route"></i>
                <h3>0</h3>
                <p>Trajets effectués</p>
            </div>
            <div class="stat-card">
                <i class="fas fa-money-bill-wave"></i>
                <h3>0 TND</h3>
                <p>Gains totaux</p>
            </div>
        </div>
        
        <div class="card">
            <h2><i class="fas fa-tasks"></i> Actions disponibles</h2>
            <div class="action-buttons">
                <div class="action-btn" onclick="alert('Fonctionnalité en cours de développement')">
                    <i class="fas fa-play-circle"></i>
                    <p><strong>Démarrer un trajet</strong></p>
                </div>
                <div class="action-btn" onclick="alert('Fonctionnalité en cours de développement')">
                    <i class="fas fa-check-circle"></i>
                    <p><strong>Terminer un trajet</strong></p>
                </div>
                <div class="action-btn" onclick="alert('Fonctionnalité en cours de développement')">
                    <i class="fas fa-history"></i>
                    <p><strong>Historique des trajets</strong></p>
                </div>
            </div>
        </div>
)xxx";
    } else if (session->role == "cashier") {
        html << R"xxx(
            <div class="stat-card">
                <i class="fas fa-building"></i>
                <h3>N/A</h3>
                <p>Station</p>
            </div>
            <div class="stat-card">
                <i class="fas fa-ticket-alt"></i>
                <h3>0</h3>
                <p>Tickets vendus</p>
            </div>
            <div class="stat-card">
                <i class="fas fa-money-bill-wave"></i>
                <h3>0 TND</h3>
                <p>Montant collecté</p>
            </div>
        </div>
        
        <div class="card">
            <h2><i class="fas fa-tasks"></i> Actions disponibles</h2>
            <div class="action-buttons">
                <div class="action-btn" onclick="alert('Fonctionnalité en cours de développement')">
                    <i class="fas fa-credit-card"></i>
                    <p><strong>Traiter un paiement</strong></p>
                </div>
                <div class="action-btn" onclick="alert('Fonctionnalité en cours de développement')">
                    <i class="fas fa-ticket-alt"></i>
                    <p><strong>Émettre un ticket</strong></p>
                </div>
                <div class="action-btn" onclick="alert('Fonctionnalité en cours de développement')">
                    <i class="fas fa-chart-line"></i>
                    <p><strong>Résumé quotidien</strong></p>
                </div>
            </div>
        </div>
)xxx";
    } else if (session->role == "manager") {
        html << R"xxx(
            <div class="stat-card">
                <i class="fas fa-building"></i>
                <h3>0</h3>
                <p>Stations gérées</p>
            </div>
            <div class="stat-card">
                <i class="fas fa-bus"></i>
                <h3>0</h3>
                <p>Louages actifs</p>
            </div>
            <div class="stat-card">
                <i class="fas fa-users"></i>
                <h3>0</h3>
                <p>Employés</p>
            </div>
        </div>
        
        <div class="card">
            <h2><i class="fas fa-tasks"></i> Actions disponibles</h2>
            <div class="action-buttons">
                <div class="action-btn" onclick="alert('Fonctionnalité en cours de développement')">
                    <i class="fas fa-chart-bar"></i>
                    <p><strong>Rapports</strong></p>
                </div>
                <div class="action-btn" onclick="alert('Fonctionnalité en cours de développement')">
                    <i class="fas fa-cog"></i>
                    <p><strong>Gérer les louages</strong></p>
                </div>
                <div class="action-btn" onclick="alert('Fonctionnalité en cours de développement')">
                    <i class="fas fa-users-cog"></i>
                    <p><strong>Gérer le personnel</strong></p>
                </div>
            </div>
        </div>
)xxx";
    } else if (session->role == "admin") {
        html << R"xxx(
            <div class="stat-card">
                <i class="fas fa-users"></i>
                <h3>0</h3>
                <p>Utilisateurs totaux</p>
            </div>
            <div class="stat-card">
                <i class="fas fa-building"></i>
                <h3>0</h3>
                <p>Stations</p>
            </div>
            <div class="stat-card">
                <i class="fas fa-bus"></i>
                <h3>0</h3>
                <p>Louages</p>
            </div>
        </div>
        
        <div class="card">
            <h2><i class="fas fa-tasks"></i> Actions disponibles</h2>
            <div class="action-buttons">
                <div class="action-btn" onclick="alert('Fonctionnalité en cours de développement')">
                    <i class="fas fa-users-cog"></i>
                    <p><strong>Gérer les utilisateurs</strong></p>
                </div>
                <div class="action-btn" onclick="alert('Fonctionnalité en cours de développement')">
                    <i class="fas fa-database"></i>
                    <p><strong>Sauvegarde système</strong></p>
                </div>
                <div class="action-btn" onclick="alert('Fonctionnalité en cours de développement')">
                    <i class="fas fa-file-alt"></i>
                    <p><strong>Logs système</strong></p>
                </div>
            </div>
        </div>
)xxx";
    }
    
    html << R"xxx(
    </div>
</body>
</html>)xxx";
    
    return html.str();
}

// ADD THIS TO html_templates.cpp (CORRECTED VERSION)
string getReservationPage(Session* session, System& system) {
    stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html lang=\"fr\">\n";
    html << "<head>\n";
    html << "    <meta charset=\"UTF-8\">\n";
    html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "    <title>Nouvelle Réservation - Louage Tracker</title>\n";
    html << "    <link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css\">\n";
    html << "    <style>" << getCommonCSS();
    html << "        .reservation-form {\n";
    html << "            background: white; border-radius: 15px; padding: 40px;\n";
    html << "            box-shadow: 0 20px 40px rgba(0,0,0,0.1); max-width: 600px; margin: 0 auto;\n";
    html << "        }\n";
    html << "        .reservation-form h2 { color: #333; margin-bottom: 30px; text-align: center; }\n";
    html << "        .success-message {\n";
    html << "            background: #d4edda; color: #155724; padding: 15px;\n";
    html << "            border-radius: 8px; margin-bottom: 20px; text-align: center;\n";
    html << "        }\n";
    html << "    </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    
    html << getNavbar(session);
    
    html << "    <div class=\"container\">\n";
    html << "        <div class=\"reservation-form\">\n";
    html << "            <h2><i class=\"fas fa-ticket-alt\"></i> Nouvelle Réservation</h2>\n";
    html << "            <div id=\"message\"></div>\n";
    html << "            <form id=\"reservationForm\" onsubmit=\"return makeReservation(event)\">\n";
    html << "                <div class=\"form-group\">\n";
    html << "                    <label for=\"fromStation\"><i class=\"fas fa-map-marker-alt\"></i> Station de départ</label>\n";
    html << "                    <select id=\"fromStation\" class=\"form-control\" required>\n";
    html << "                        <option value=\"\">-- Sélectionnez --</option>\n";
    
    vector<string> stations = system.getAllStationNames();
    sort(stations.begin(), stations.end());
    for (const auto& station : stations) {
        html << "                        <option value=\"" << escapeHTML(station) << "\">" 
             << escapeHTML(station) << "</option>\n";
    }
    
    html << "                    </select>\n";
    html << "                </div>\n";
    html << "                <div class=\"form-group\">\n";
    html << "                    <label for=\"toDestination\"><i class=\"fas fa-flag-checkered\"></i> Destination</label>\n";
    html << "                    <select id=\"toDestination\" class=\"form-control\" required disabled>\n";
    html << "                        <option value=\"\">-- Sélectionnez d'abord le départ --</option>\n";
    html << "                    </select>\n";
    html << "                </div>\n";
    html << "                <div class=\"form-group\">\n";
    html << "                    <label for=\"passengerName\"><i class=\"fas fa-user\"></i> Nom du passager</label>\n";
    html << "                    <input type=\"text\" id=\"passengerName\" class=\"form-control\" value=\"" 
         << escapeHTML(session->username) << "\" required>\n";
    html << "                </div>\n";
    html << "                <button type=\"submit\" class=\"btn\" style=\"width: 100%;\">\n";
    html << "                    <i class=\"fas fa-check\"></i> Confirmer la réservation\n";
    html << "                </button>\n";
    html << "                <a href=\"/dashboard\" class=\"btn\" style=\"width: 100%; margin-top: 10px; background: #6c757d;\">\n";
    html << "                    <i class=\"fas fa-arrow-left\"></i> Retour au dashboard\n";
    html << "                </a>\n";
    html << "            </form>\n";
    html << "        </div>\n";
    html << "    </div>\n";
    html << "    <script>\n";
    html << "        document.getElementById('fromStation').addEventListener('change', function() {\n";
    html << "            const fromStation = this.value;\n";
    html << "            const toSelect = document.getElementById('toDestination');\n";
    html << "            if (!fromStation) {\n";
    html << "                toSelect.innerHTML = '<option value=\"\">-- Sélectionnez d\\'abord le départ --</option>';\n";
    html << "                toSelect.disabled = true; return;\n";
    html << "            }\n";
    html << "            toSelect.innerHTML = '<option value=\"\">-- Chargement... --</option>';\n";
    html << "            toSelect.disabled = true;\n";
    html << "            fetch('/api/destinations?station=' + encodeURIComponent(fromStation))\n";
    html << "                .then(response => response.json())\n";
    html << "                .then(data => {\n";
    html << "                    toSelect.innerHTML = '<option value=\"\">-- Sélectionnez une destination --</option>';\n";
    html << "                    if (data.destinations && data.destinations.length > 0) {\n";
    html << "                        const uniqueDests = [...new Set(data.destinations)].sort();\n";
    html << "                        uniqueDests.forEach(dest => {\n";
    html << "                            const option = document.createElement('option');\n";
    html << "                            option.value = dest; option.textContent = dest;\n";
    html << "                            toSelect.appendChild(option);\n";
    html << "                        });\n";
    html << "                        toSelect.disabled = false;\n";
    html << "                    }\n";
    html << "                })\n";
    html << "                .catch(error => {\n";
    html << "                    console.error('Error:', error);\n";
    html << "                    toSelect.innerHTML = '<option value=\"\">-- Erreur --</option>';\n";
    html << "                });\n";
    html << "        });\n";
    html << "        function makeReservation(event) {\n";
    html << "            event.preventDefault();\n";
    html << "            const fromStation = document.getElementById('fromStation').value;\n";
    html << "            const toDestination = document.getElementById('toDestination').value;\n";
    html << "            const passengerName = document.getElementById('passengerName').value;\n";
    html << "            fetch('/api/reserve', {\n";
    html << "                method: 'POST',\n";
    html << "                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },\n";
    html << "                body: 'from=' + encodeURIComponent(fromStation) + '&to=' + encodeURIComponent(toDestination) + '&name=' + encodeURIComponent(passengerName)\n";
    html << "            })\n";
    html << "            .then(response => response.json())\n";
    html << "            .then(data => {\n";
    html << "                if (data.success) {\n";
    html << "                    document.getElementById('message').innerHTML = '<div class=\"success-message\"><i class=\"fas fa-check-circle\"></i><p><strong>Réservation confirmée!</strong></p><p>Numéro: <strong>#' + data.reservationNumber + '</strong></p></div>';\n";
    html << "                    document.getElementById('reservationForm').reset();\n";
    html << "                } else {\n";
    html << "                    document.getElementById('message').innerHTML = '<div class=\"alert alert-error\"><i class=\"fas fa-exclamation-circle\"></i><p>Erreur: ' + data.error + '</p></div>';\n";
    html << "                }\n";
    html << "            })\n";
    html << "            .catch(error => {\n";
    html << "                document.getElementById('message').innerHTML = '<div class=\"alert alert-error\"><i class=\"fas fa-exclamation-circle\"></i><p>Erreur: ' + error.message + '</p></div>';\n";
    html << "            });\n";
    html << "            return false;\n";
    html << "        }\n";
    html << "    </script>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

string getMyReservationsPage(Session* session, System& system) {
    stringstream html;
    html << "<!DOCTYPE html>\n<html lang=\"fr\">\n<head>\n";
    html << "<meta charset=\"UTF-8\">\n";
    html << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "<title>Mes Réservations - Louage Tracker</title>\n";
    html << "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css\">\n";
    html << "<style>" << getCommonCSS();
    html << ".reservation-card { background: white; border-radius: 10px; padding: 20px; margin-bottom: 15px; box-shadow: 0 2px 8px rgba(0,0,0,0.1); }\n";
    html << ".reservation-card h3 { color: #667eea; margin-bottom: 10px; }\n";
    html << ".status-badge { display: inline-block; padding: 5px 15px; border-radius: 20px; font-size: 0.9rem; font-weight: bold; }\n";
    html << ".status-waiting { background: #fff3cd; color: #856404; }\n";
    html << ".status-paid { background: #d4edda; color: #155724; }\n";
    html << ".status-completed { background: #d1ecf1; color: #0c5460; }\n";
    html << ".btn-cancel { background: #dc3545; padding: 8px 20px; color: white; border: none; border-radius: 5px; cursor: pointer; }\n";
    html << ".btn-pay { background: #28a745; padding: 8px 20px; color: white; border: none; border-radius: 5px; cursor: pointer; }\n";
    html << ".empty-state { text-align: center; padding: 60px 20px; color: #666; }\n";
    html << ".empty-state i { font-size: 4rem; color: #667eea; margin-bottom: 20px; }\n";
    html << "</style>\n</head>\n<body>\n";
    
    html << getNavbar(session);
    
    html << "<div class=\"container\">\n";
    html << "<div class=\"card\">\n";
    html << "<h1><i class=\"fas fa-list\"></i> Mes Réservations</h1>\n";
    html << "<div id=\"reservationsList\"></div>\n";
    html << "<a href=\"/dashboard\" class=\"btn\" style=\"margin-top: 20px;\"><i class=\"fas fa-arrow-left\"></i> Retour</a>\n";
    html << "</div>\n</div>\n";
    
    html << "<script>\n";
    html << "function loadReservations() {\n";
    html << "  fetch('/api/my-reservations')\n";
    html << "    .then(r => r.json())\n";
    html << "    .then(data => {\n";
    html << "      const list = document.getElementById('reservationsList');\n";
    html << "      if (data.reservations && data.reservations.length > 0) {\n";
    html << "        let html = '';\n";
    html << "        data.reservations.forEach(r => {\n";
    html << "          const statusClass = r.paid ? 'status-paid' : 'status-waiting';\n";
    html << "          const statusText = r.paid ? 'Payé' : 'En attente';\n";
    html << "          html += '<div class=\"reservation-card\">';\n";
    html << "          html += '<h3><i class=\"fas fa-ticket-alt\"></i> Réservation #' + r.number + '</h3>';\n";
    html << "          html += '<p><strong>' + r.from + ' → ' + r.to + '</strong></p>';\n";
    html << "          html += '<p>Passager: ' + r.name + '</p>';\n";
    html << "          html += '<p>Appelé: ' + r.timesCalled + ' fois</p>';\n";
    html << "          html += '<span class=\"status-badge ' + statusClass + '\">' + statusText + '</span> ';\n";
    html << "          if (!r.paid) {\n";
    html << "            html += '<button class=\"btn-pay\" onclick=\"payReservation(' + r.number + ')\"><i class=\"fas fa-credit-card\"></i> Payer</button> ';\n";
    html << "            html += '<button class=\"btn-cancel\" onclick=\"cancelReservation(' + r.number + ')\"><i class=\"fas fa-times\"></i> Annuler</button>';\n";
    html << "          }\n";
    html << "          html += '</div>';\n";
    html << "        });\n";
    html << "        list.innerHTML = html;\n";
    html << "      } else {\n";
    html << "        list.innerHTML = '<div class=\"empty-state\"><i class=\"fas fa-inbox\"></i><h3>Aucune réservation</h3><p>Vous n\\'avez pas encore de réservations.</p><a href=\"/reservation\" class=\"btn\"><i class=\"fas fa-plus\"></i> Créer une réservation</a></div>';\n";
    html << "      }\n";
    html << "    })\n";
    html << "    .catch(e => {\n";
    html << "      document.getElementById('reservationsList').innerHTML = '<div class=\"alert alert-error\">Erreur: ' + e.message + '</div>';\n";
    html << "    });\n";
    html << "}\n";
    
    html << "function payReservation(number) {\n";
    html << "  if (!confirm('Confirmer le paiement pour la réservation #' + number + '?')) return;\n";
    html << "  fetch('/api/pay-reservation', {\n";
    html << "    method: 'POST',\n";
    html << "    headers: {'Content-Type': 'application/x-www-form-urlencoded'},\n";
    html << "    body: 'number=' + number\n";
    html << "  })\n";
    html << "  .then(r => r.json())\n";
    html << "  .then(data => {\n";
    html << "    if (data.success) {\n";
    html << "      alert('Paiement confirmé!');\n";
    html << "      loadReservations();\n";
    html << "    } else {\n";
    html << "      alert('Erreur: ' + data.error);\n";
    html << "    }\n";
    html << "  });\n";
    html << "}\n";
    
    html << "function cancelReservation(number) {\n";
    html << "  if (!confirm('Annuler la réservation #' + number + '?')) return;\n";
    html << "  fetch('/api/cancel-reservation', {\n";
    html << "    method: 'POST',\n";
    html << "    headers: {'Content-Type': 'application/x-www-form-urlencoded'},\n";
    html << "    body: 'number=' + number\n";
    html << "  })\n";
    html << "  .then(r => r.json())\n";
    html << "  .then(data => {\n";
    html << "    if (data.success) {\n";
    html << "      alert('Réservation annulée!');\n";
    html << "      loadReservations();\n";
    html << "    } else {\n";
    html << "      alert('Erreur: ' + data.error);\n";
    html << "    }\n";
    html << "  });\n";
    html << "}\n";
    
    html << "loadReservations();\n";
    html << "</script>\n</body>\n</html>\n";
    
    return html.str();
}

string getPassengerProfilePage(Session* session, AccountManager& accountManager) {
    stringstream html;
    html << "<!DOCTYPE html>\n<html lang=\"fr\">\n<head>\n";
    html << "<meta charset=\"UTF-8\">\n";
    html << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "<title>Mon Profil - Louage Tracker</title>\n";
    html << "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css\">\n";
    html << "<style>" << getCommonCSS();
    html << ".profile-header { background: linear-gradient(135deg, #667eea, #764ba2); color: white; padding: 30px; border-radius: 10px; margin-bottom: 30px; text-align: center; }\n";
    html << ".profile-header i { font-size: 4rem; margin-bottom: 15px; }\n";
    html << ".info-row { display: flex; justify-content: space-between; padding: 15px 0; border-bottom: 1px solid #e0e0e0; }\n";
    html << ".info-label { font-weight: bold; color: #666; }\n";
    html << ".info-value { color: #333; }\n";
    html << "</style>\n</head>\n<body>\n";
    
    html << getNavbar(session);
    
    html << "<div class=\"container\">\n";
    html << "<div class=\"card\">\n";
    html << "<div class=\"profile-header\">\n";
    html << "<i class=\"fas fa-user-circle\"></i>\n";
    html << "<h1>" << escapeHTML(session->username) << "</h1>\n";
    html << "<p>Compte " << escapeHTML(session->role) << "</p>\n";
    html << "</div>\n";
    
    html << "<h2><i class=\"fas fa-info-circle\"></i> Informations du compte</h2>\n";
    html << "<div class=\"info-row\"><span class=\"info-label\">Nom d'utilisateur:</span><span class=\"info-value\">" << escapeHTML(session->username) << "</span></div>\n";
    html << "<div class=\"info-row\"><span class=\"info-label\">Rôle:</span><span class=\"info-value\">" << escapeHTML(session->role) << "</span></div>\n";
    html << "<div class=\"info-row\"><span class=\"info-label\">ID:</span><span class=\"info-value\" id=\"userId\">-</span></div>\n";
    
    html << "<h2 style=\"margin-top: 30px;\"><i class=\"fas fa-chart-bar\"></i> Statistiques</h2>\n";
    html << "<div id=\"stats\"></div>\n";
    
    html << "<div style=\"margin-top: 30px;\">\n";
    html << "<a href=\"/dashboard\" class=\"btn\"><i class=\"fas fa-arrow-left\"></i> Retour</a>\n";
    html << "<button class=\"btn\" style=\"background: #dc3545; margin-left: 10px;\" onclick=\"if(confirm('Changer le mot de passe?')) window.location='/change-password';\"><i class=\"fas fa-key\"></i> Changer mot de passe</button>\n";
    html << "</div>\n";
    html << "</div>\n</div>\n";
    
    html << "<script>\n";
    html << "fetch('/api/my-profile')\n";
    html << "  .then(r => r.json())\n";
    html << "  .then(data => {\n";
    html << "    if (data.success) {\n";
    html << "      document.getElementById('userId').textContent = data.userId;\n";
    html << "      document.getElementById('stats').innerHTML = '\n";
    html << "        <div class=\"info-row\"><span class=\"info-label\">Total réservations:</span><span class=\"info-value\">' + (data.totalReservations || 0) + '</span></div>\n";
    html << "        <div class=\"info-row\"><span class=\"info-label\">Réservations actives:</span><span class=\"info-value\">' + (data.activeReservations || 0) + '</span></div>\n";
    html << "        <div class=\"info-row\"><span class=\"info-label\">Trajets terminés:</span><span class=\"info-value\">' + (data.completedTrips || 0) + '</span></div>';\n";
    html << "    }\n";
    html << "  });\n";
    html << "</script>\n</body>\n</html>\n";
    
    return html.str();
}