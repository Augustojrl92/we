#include "../includes/UploadHandler.hpp"
#include "../includes/utils.hpp"
#include <sstream>
#include <iostream>
#include <sys/stat.h>


UploadHandler::UploadHandler(const Request& request, const Config& cfg)
: req(request), config(cfg) {}

bool UploadHandler::isUploadRequest(const Request& req) {
    return req.method == "POST" &&
           req.headers.count("content-type") &&
           req.headers.at("content-type").find("multipart/form-data") != std::string::npos;
}

std::string UploadHandler::extractBoundary() const {
    std::string contentType = req.headers.at("content-type");
    size_t pos = contentType.find("boundary=");
    if (pos == std::string::npos) return "";
    return "--" + contentType.substr(pos + 9);
}

std::string UploadHandler::saveFile(const std::string& filename, const std::string& content) const {
    std::string uploadPath = config.getUploadStore();
    if (uploadPath.empty()) uploadPath = "www/uploads";

    mkdir(uploadPath.c_str(), 0755); // asegura que exista la carpeta
    std::string path = uploadPath + "/" + filename;

    if (!writeFileNonBlocking(path, content))
        throw std::runtime_error("Error writing file: " + path);

    return path;
}

std::string UploadHandler::generateHTMLSuccess(const std::string& filename, size_t size) const {
    std::string templatePath = "www/upload_success.html";
    std::string html = readFileNonBlocking(templatePath);

    if (html.empty()) {
        // Si no se encuentra el archivo, usar fallback interno
        std::ostringstream fallback;
        fallback << "<!DOCTYPE html><html><body>"
                 << "<h1>✅ Upload Successful</h1>"
                 << "<p>File saved as: " << filename << "</p>"
                 << "<p>Size: " << size << " bytes</p>"
                 << "</body></html>";
        return fallback.str();
    }

    // Reemplazar placeholders
    size_t pos;
    if ((pos = html.find("{{FILENAME}}")) != std::string::npos)
        html.replace(pos, 12, filename);

    std::ostringstream sizeStr;
    sizeStr << size;
    if ((pos = html.find("{{SIZE}}")) != std::string::npos)
        html.replace(pos, 8, sizeStr.str());

    return html;
}


std::string UploadHandler::generateHTMLError(const std::string& msg) const {
    std::ostringstream html;
    html << "<!DOCTYPE html><html><body><h1>❌ " << msg << "</h1></body></html>";
    return html.str();
}

Response UploadHandler::handle() {
    Response res;
    std::string boundary = extractBoundary();

    if (boundary.empty()) {
        res.status_code = 400;
        res.body = generateHTMLError("Missing boundary");
        return res;
    }

    size_t start = req.body.find(boundary);
    while (start != std::string::npos) {
        size_t end = req.body.find(boundary, start + boundary.size());
        if (end == std::string::npos) break;

        std::string part = req.body.substr(start + boundary.size(),
                                           end - (start + boundary.size()));

        size_t filenamePos = part.find("filename=\"");
        if (filenamePos != std::string::npos) {
            filenamePos += 10;
            size_t filenameEnd = part.find("\"", filenamePos);
            std::string filename = part.substr(filenamePos, filenameEnd - filenamePos);

            size_t dataPos = part.find("\r\n\r\n");
            if (dataPos != std::string::npos) {
                std::string fileContent = part.substr(dataPos + 4);
                if (fileContent.size() >= 2 &&
                    fileContent.substr(fileContent.size() - 2) == "\r\n")
                    fileContent = fileContent.substr(0, fileContent.size() - 2);

                try {
                    saveFile(filename, fileContent);
                    res.status_code = 201;
                    res.body = generateHTMLSuccess(filename, fileContent.size());
                } catch (std::exception &e) {
                    res.status_code = 500;
                    res.body = generateHTMLError(e.what());
                }
            }
        }
        start = end;
    }

    res.headers["Content-Type"] = "text/html; charset=UTF-8";
    std::ostringstream oss;
    oss << res.body.size();
    res.headers["Content-Length"] = oss.str();
    return res;
}
