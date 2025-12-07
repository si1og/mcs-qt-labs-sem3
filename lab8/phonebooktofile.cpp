#include "phonebooktofile.h"
#include <QDataStream>
#include <yaml-cpp/yaml.h>
#include <fstream>

PhoneBookFile::PhoneBookFile(const QString& fileName, QObject* parent)
    : QFile(fileName, parent) {}

bool PhoneBookFile::loadContacts(QList<Contact>& contacts) {
    contacts.clear();

    if (!exists()) {
        return true;
    }

    if (!open(QIODevice::ReadOnly)) {
        m_lastError = QString("Не удалось открыть файл: %1").arg(errorString());
        return false;
    }

    QDataStream in(this);
    in.setVersion(QDataStream::Qt_5_15);

    quint32 magic, version;
    in >> magic >> version;

    if (magic != FILE_MAGIC) {
        m_lastError = "Неверный формат файла";
        close();
        return false;
    }

    if (version > FILE_VERSION) {
        m_lastError = "Файл создан более новой версией программы";
        close();
        return false;
    }

    in >> contacts;

    if (in.status() != QDataStream::Ok) {
        m_lastError = "Ошибка чтения данных";
        close();
        return false;
    }

    close();
    return true;
}

bool PhoneBookFile::saveContacts(const QList<Contact>& contacts) {
    if (!open(QIODevice::WriteOnly)) {
        m_lastError = QString("Не удалось открыть файл для записи: %1").arg(errorString());
        return false;
    }

    QDataStream out(this);
    out.setVersion(QDataStream::Qt_5_15);

    out << FILE_MAGIC << FILE_VERSION;

    out << contacts;

    if (out.status() != QDataStream::Ok) {
        m_lastError = "Ошибка записи данных";
        close();
        return false;
    }

    close();
    return true;
}

bool PhoneBookFile::loadContactsYaml(QList<Contact>& contacts) {
    contacts.clear();
    
    QString path = fileName();
    if (!QFile::exists(path)) {
        return true;
    }
    
    try {
        YAML::Node root = YAML::LoadFile(path.toStdString());
        
        if (!root.IsSequence()) {
            m_lastError = "Неверный формат YAML: ожидался список контактов";
            return false;
        }
        
        for (const auto& node : root) {
            Contact contact;
            
            if (node["firstName"]) {
                contact.firstName = QString::fromStdString(node["firstName"].as<std::string>());
            }
            if (node["lastName"]) {
                contact.lastName = QString::fromStdString(node["lastName"].as<std::string>());
            }
            if (node["middleName"]) {
                contact.middleName = QString::fromStdString(node["middleName"].as<std::string>());
            }
            if (node["address"]) {
                contact.address = QString::fromStdString(node["address"].as<std::string>());
            }
            if (node["email"]) {
                contact.email = QString::fromStdString(node["email"].as<std::string>());
            }
            if (node["birthDate"]) {
                QString dateStr = QString::fromStdString(node["birthDate"].as<std::string>());
                contact.birthDate = QDate::fromString(dateStr, "yyyy-MM-dd");
            }
            
            if (node["phones"] && node["phones"].IsSequence()) {
                for (const auto& phoneNode : node["phones"]) {
                    PhoneNumber phone;
                    
                    if (phoneNode["number"]) {
                        phone.number = QString::fromStdString(phoneNode["number"].as<std::string>());
                    }
                    if (phoneNode["type"]) {
                        std::string typeStr = phoneNode["type"].as<std::string>();
                        if (typeStr == "work") {
                            phone.type = PhoneNumber::Work;
                        } else if (typeStr == "home") {
                            phone.type = PhoneNumber::Home;
                        } else {
                            phone.type = PhoneNumber::Mobile;
                        }
                    }
                    
                    contact.phones.append(phone);
                }
            }
            
            contacts.append(contact);
        }
        
        return true;
        
    } catch (const YAML::Exception& e) {
        m_lastError = QString("Ошибка парсинга YAML: %1").arg(e.what());
        return false;
    }
}

bool PhoneBookFile::saveContactsYaml(const QList<Contact>& contacts) {
    QString path = fileName();
    
    try {
        YAML::Emitter out;
        out << YAML::BeginSeq;
        
        for (const Contact& contact : contacts) {
            out << YAML::BeginMap;
            
            out << YAML::Key << "lastName" << YAML::Value << contact.lastName.toStdString();
            out << YAML::Key << "firstName" << YAML::Value << contact.firstName.toStdString();
            out << YAML::Key << "middleName" << YAML::Value << contact.middleName.toStdString();
            out << YAML::Key << "address" << YAML::Value << contact.address.toStdString();
            out << YAML::Key << "email" << YAML::Value << contact.email.toStdString();
            out << YAML::Key << "birthDate" << YAML::Value << contact.birthDate.toString("yyyy-MM-dd").toStdString();
            
            out << YAML::Key << "phones" << YAML::Value << YAML::BeginSeq;
            for (const PhoneNumber& phone : contact.phones) {
                out << YAML::BeginMap;
                
                std::string typeStr;
                switch (phone.type) {
                    case PhoneNumber::Work: typeStr = "work"; break;
                    case PhoneNumber::Home: typeStr = "home"; break;
                    default: typeStr = "mobile"; break;
                }
                
                out << YAML::Key << "type" << YAML::Value << typeStr;
                out << YAML::Key << "number" << YAML::Value << phone.number.toStdString();
                out << YAML::EndMap;
            }
            out << YAML::EndSeq;
            
            out << YAML::EndMap;
        }
        
        out << YAML::EndSeq;
        
        std::ofstream fout(path.toStdString());
        if (!fout.is_open()) {
            m_lastError = "Не удалось открыть файл для записи";
            return false;
        }
        
        fout << out.c_str();
        fout.close();
        
        return true;
        
    } catch (const YAML::Exception& e) {
        m_lastError = QString("Ошибка записи YAML: %1").arg(e.what());
        return false;
    }
}