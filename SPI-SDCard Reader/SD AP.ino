#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <WebServer.h>

#define CS_PIN 5 // Define el pin de selección de chip para la tarjeta SD

WebServer server(80); // Crea un servidor web en el puerto 80

String currentPath = "/"; // Ruta actual

void handleRoot() {
  String html = "<html><head>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; margin: 20px; }";
  html += "h1 { text-align: center; }";
  html += "table { width: 100%; border-collapse: collapse; }";
  html += "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }";
  html += "th { background-color: #f2f2f2; }";
  html += "tr:hover { background-color: #f5f5f5; }";
  html += ".dropdown { position: relative; display: inline-block; }";
  html += ".dropdown-content { display: none; position: absolute; background-color: #f9f9f9; min-width: 160px; box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2); z-index: 1; }";
  html += ".dropdown-content a { color: black; padding: 12px 16px; text-decoration: none; display: block; }";
  html += ".dropdown-content a:hover { background-color: #f1f1f1; }";
  html += ".dropdown:hover .dropdown-content { display: block; }";
  html += "</style>";
  html += "</head><body>";
  html += "<h1>Gestión de Archivos SD</h1>";
  
  html += "<form action='/upload' method='post' enctype='multipart/form-data'>";
  html += "Archivo: <input type='file' name='data'><br>";
  html += "<input type='submit' value='Subir'>";
  html += "</form>";

  html += "<form action='/createfolder' method='post'>";
  html += "Nombre de la carpeta: <input type='text' name='foldername'><br>";
  html += "<input type='submit' value='Crear Carpeta'>";
  html += "</form>";
  
  html += "<h2>Archivos en " + currentPath + "</h2>";
  html += "<table>";
  html += "<tr><th>Nombre</th><th>Opciones</th></tr>";

  File dir = SD.open(currentPath);
  File file = dir.openNextFile();
  
  while (file) {
    if (file.isDirectory()) {
      html += "<tr><td><a href='?path=" + String(file.name()) + "'>" + String(file.name()) + "</a></td>";
      html += "<td class='dropdown'><button>Opciones</button>";
      html += "<div class='dropdown-content'>";
      html += "<a href='/delete?filename=" + String(file.name()) + "'>Eliminar Carpeta</a>";
      html += "<a href='/rename?filename=" + String(file.name()) + "'>Renombrar Carpeta</a>";
      html += "</div></td></tr>";
    } else {
      html += "<tr><td>" + String(file.name()) + "</td>";
      html += "<td class='dropdown'><button>Opciones</button>";
      html += "<div class='dropdown-content'>";
      html += "<a href='/download?file=" + String(file.name()) + "'>Descargar</a>";
      html += "<a href='/delete?filename=" + String(file.name()) + "'>Eliminar</a>";
      html += "</div></td></tr>";
    }
    file = dir.openNextFile();
  }
  
  html += "</table>";
  html += "<br><a href='?path=/'>Volver al Directorio Raíz</a>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleFileUpload() {
  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = "/" + upload.filename;
    Serial.print("Subiendo archivo: ");
    Serial.println(filename);
    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
      Serial.println("Error al abrir el archivo para escritura");
      return;
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    File file = SD.open(upload.filename, FILE_WRITE);
    if (file) {
      file.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    File file = SD.open(upload.filename, FILE_WRITE);
    if (file) {
      file.close();
    }
    Serial.print("Archivo subido: ");
    Serial.println(upload.filename);
  }
}

void handleDeleteFile() {
  String filename = server.arg("filename");
  if (SD.remove("/" + filename)) {
    server.send(200, "text/html", "Archivo eliminado correctamente.<br><a href='/'>Volver</a>");
  } else {
    server.send(200, "text/html", "Error al eliminar el archivo.<br><a href='/'>Volver</a>");
  }
}

void handleDownloadFile() {
  String filename = server.arg("file");
  File file = SD.open("/" + filename, FILE_READ);
  if (file) {
    server.streamFile(file, "application/octet-stream");
    file.close();
  } else {
    server.send(404, "text/plain", "Archivo no encontrado");
  }
}

void handleCreateFolder() {
  String folderName = server.arg("foldername");
  if (SD.mkdir("/" + folderName)) {
    server.send(200, "text/html", "Carpeta creada correctamente.<br><a href='/'>Volver</a>");
  } else {
    server.send(200, "text/html", "Error al crear la carpeta.<br><a href='/'>Volver</a>");
  }
}

void handleRenameFile() {
  String oldName = server.arg("filename");
  String newName = server.arg("newname");
  if (SD.rename("/" + oldName, "/" + newName)) {
    server.send(200, "text/html", "Archivo/Carpeta renombrado correctamente.<br><a href='/'>Volver</a>");
  } else {
    server.send(200, "text/html", "Error al renombrar el archivo/carpeta.<br><a href='/'>Volver</a>");
  }
}

void handleDeleteFolder() {
  String folderName = server.arg("filename");
  if (SD.rmdir("/" + folderName)) {
    server.send(200, "text/html", "Carpeta eliminada correctamente.<br><a href='/'>Volver</a>");
  } else {
    server.send(200, "text/html", "Error al eliminar la carpeta.<br><a href='/'>Volver</a>");
  }
}

void handleFileList() {
  currentPath = server.arg("path");
  handleRoot();
}

void handleSDInfo() {
  String html = "<html><body>";
  html += "<h1>Información de la Tarjeta SD</h1>";
  
  // Obtener información de la tarjeta SD
  uint64_t totalBytes = SD.cardSize(); // Este método puede no estar disponible; usa otro método si es necesario
  uint64_t usedBytes = 0;
  uint64_t freeBytes = 0;
  
  File root = SD.open("/");
  File file = root.openNextFile();
  while (file) {
    usedBytes += file.size();
    file = root.openNextFile();
  }

  freeBytes = totalBytes - usedBytes;

  html += "Total bytes: " + String(totalBytes) + "<br>";
  html += "Usado bytes: " + String(usedBytes) + "<br>";
  html += "Libre bytes: " + String(freeBytes) + "<br>";
  html += "<a href='/'>Volver</a>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  
  // Configura el ESP32 como punto de acceso
  WiFi.softAP("ESP32_SD_AP", "123456789");
  Serial.println("Punto de acceso iniciado");
  
  // Configura la tarjeta SD
  if (!SD.begin(CS_PIN)) {
    Serial.println("Fallo al inicializar la tarjeta SD");
    return;
  }

  Serial.println("Tarjeta SD inicializada");

  // Configura las rutas del servidor web
  server.on("/", handleRoot);
  server.on("/upload", HTTP_POST, []() {}, handleFileUpload);
  server.on("/delete", HTTP_POST, handleDeleteFile);
  server.on("/download", handleDownloadFile);
  server.on("/createfolder", HTTP_POST, handleCreateFolder);
  server.on("/rename", HTTP_POST, handleRenameFile);
  server.on("/deletefolder", HTTP_POST, handleDeleteFolder);
  server.on("/files", handleFileList);
  server.on("/sdinfo", handleSDInfo);

  server.begin();
}

void loop() {
  server.handleClient();
}
