#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "notePad"
#define APPSK ""
#endif

const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

// Initial content for ch[]
char ch[10000] = "hello world";

void handleRoot() {
  String content = "<html><body>";
  content += "<h1>Edit Content</h1>";
  content += "<form action='/' method='post'>";
  content += "<input type='text' name='newContent' value='";
  content += ch;
  content += "'><br>";
  content += "<input type='submit' value='OK'>";
  content += "<input type='button' value='Reset' onclick='resetContent()'>";
  content += "<input type='button' value='Copy Text' onclick='copyText()'>"; // New button for copying text
  content += "</form>";
  content += "<script>";
  content += "function resetContent() {";
  content += "  var xhr = new XMLHttpRequest();";
  content += "  xhr.open('GET', '/reset', true);";
  content += "  xhr.send();";
  content += "  xhr.onload = function() {";
  content += "    if (xhr.status == 200) {";
  content += "      window.location = 'http://192.168.4.1/';";
  content += "    }";
  content += "  };";
  content += "}";
  content += "function copyText() {";
  content += "  var textField = document.createElement('textarea');";
  content += "  textField.innerText = '";
  content += ch;
  content += "';";
  content += "  document.body.appendChild(textField);";
  content += "  textField.select();";
  content += "  document.execCommand('copy');";
  content += "  textField.remove();";
  content += "}";
  content += "</script>";
  content += "</body></html>";

  server.send(200, "text/html", content);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.on("/", HTTP_GET, handleRoot);
  server.on("/", HTTP_POST, []() {
    String newContent = server.arg("newContent");
    newContent.toCharArray(ch, sizeof(ch));
    handleRoot();
  });

  server.on("/reset", HTTP_GET, []() {
    // Reset ch[] to initial content
    handleRoot(); // Refresh the page after resetting
  });
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
