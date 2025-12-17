FROM gcc:latest

WORKDIR /app

# Copy everything
COPY . .

# Create data directory if it doesn't exist
RUN mkdir -p data

# Compile
RUN g++ -o server \
    src/web_server_main.cpp \
    src/html_templates.cpp \
    src/System.cpp \
    src/Station.cpp \
    src/Destination.cpp \
    src/Louage.cpp \
    src/AccountManager.cpp \
    src/User.cpp \
    src/Passenger.cpp \
    src/Driver.cpp \
    src/Cashier.cpp \
    src/Manager.cpp \
    src/Admin.cpp \
    src/Reservation.cpp \
    src/ReservationQueue.cpp \
    src/ReservationSystem.cpp \
    -Iinclude \
    -std=c++17 \
    -pthread

EXPOSE 8080

CMD ["./server"]
