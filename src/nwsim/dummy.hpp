#pragma once

#include <iostream>

// qt-library headers

#include <QIODevice>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QSaveFile>
#include <QString>
#include <QJsonDocument>



class Dummy {
public:
    Dummy(int a, int b) : a_(a), b_(b) { }
    ~Dummy() { }

    bool write() const {
        QJsonObject json;
        json["a"] = QJsonValue(a_);
        json["b"] = QJsonValue(b_);

        QSaveFile sf(QStringLiteral("test.json"));

        if (!sf.open(QIODevice::WriteOnly)) {
            std::cout << "Couldn't open save file." << std::endl;
            return false;
        }

        sf.write(QJsonDocument(json).toJson());
        sf.commit();
        std::cout << "Written to Json-file succesfully. JESSE" << std::endl;
        return true;
    }

private:
    int a_;
    int b_;
};