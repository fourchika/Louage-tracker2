FROM gcc:latest

WORKDIR /app

# Copie tout le code
COPY . .

# Compile le serveur
RUN g++ -o server \
    src/*.cpp \
    -Iinclude \
    -std=c++17 \
    -pthread

# Expose le port
EXPOSE 8080

# Lance le serveur
CMD ["./server"]