echo "FROM gcc:latest" > Dockerfile
echo "" >> Dockerfile
echo "WORKDIR /app" >> Dockerfile
echo "" >> Dockerfile
echo "# Copie tous les fichiers" >> Dockerfile
echo "COPY . ." >> Dockerfile
echo "" >> Dockerfile
echo "# Compile l'application" >> Dockerfile
echo "RUN g++ -o server \" >> Dockerfile
echo "    backend/src/web_server_main.cpp \" >> Dockerfile
echo "    backend/src/html_templates.cpp \" >> Dockerfile
echo "    backend/src/System.cpp \" >> Dockerfile
echo "    backend/src/Station.cpp \" >> Dockerfile
echo "    backend/src/Destination.cpp \" >> Dockerfile
echo "    backend/src/Louage.cpp \" >> Dockerfile
echo "    backend/src/AccountManager.cpp \" >> Dockerfile
echo "    backend/src/User.cpp \" >> Dockerfile
echo "    backend/src/Passenger.cpp \" >> Dockerfile
echo "    backend/src/Driver.cpp \" >> Dockerfile
echo "    backend/src/Cashier.cpp \" >> Dockerfile
echo "    backend/src/Manager.cpp \" >> Dockerfile
echo "    backend/src/Admin.cpp \" >> Dockerfile
echo "    backend/src/Reservation.cpp \" >> Dockerfile
echo "    backend/src/ReservationQueue.cpp \" >> Dockerfile
echo "    backend/src/ReservationSystem.cpp \" >> Dockerfile
echo "    -I./backend/include \" >> Dockerfile
echo "    -std=c++11 \" >> Dockerfile
echo "    -pthread" >> Dockerfile
echo "" >> Dockerfile
echo "EXPOSE 8080" >> Dockerfile
echo "" >> Dockerfile
echo "CMD [\"./server\"]" >> Dockerfile