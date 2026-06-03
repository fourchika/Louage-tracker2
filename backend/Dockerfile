FROM gcc:12-bookworm

WORKDIR /app

RUN apt-get update && apt-get install -y \
    libpqxx-dev \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . .

RUN mkdir -p data

RUN g++ -o server \
    src/web_server_main.cpp \
    src/AccountManager.cpp \
    src/User.cpp \
    src/Admin.cpp \
    src/Cashier.cpp \
    src/Driver.cpp \
    src/Manager.cpp \
    src/Passenger.cpp \
    src/Destination.cpp \
    src/Louage.cpp \
    src/reservation.cpp \
    src/ReservationSystem.cpp \
    src/ReservationQueue.cpp \
    src/Station.cpp \
    src/System.cpp \
    src/html_templates.cpp \
    src/Database.cpp \
    -Iinclude -Isrc \
    -std=c++17 \
    -lpqxx -lpq -lssl -lcrypto \
    -pthread

EXPOSE 8080

CMD ["./server"]
