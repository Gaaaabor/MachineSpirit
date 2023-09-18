// DeviceModel.cpp
#include "DeviceModel.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "DeviceAttachment.h"
#include "DeviceService.h"

DeviceModel::DeviceModel(String &userId, String &deviceId, String &deviceSerial, String &deviceName, DeviceService &deviceService)
{
    this->userId = userId;
    this->deviceId = deviceId;
    this->deviceSerial = deviceSerial;
    this->deviceName = deviceName;
    this->attachmentSlots = 0;
    this->deviceService = &deviceService;
}

void SaveToEEPROM(void *data_source, size_t size)
{
    EEPROM.begin(512);
    delay(500);

    for (size_t i = 0; i < size; i++)
    {
        char data = ((char *)data_source)[i];
        EEPROM.write(i, data);
    }
    EEPROM.commit();
}

void LoadFromEEPROM(void *data_dest, size_t size)
{
    EEPROM.begin(512);
    delay(500);

    for (size_t i = 0; i < size; i++)
    {
        char data = EEPROM.read(i);
        ((char *)data_dest)[i] = data;
    }
}

void DeviceModel::Tell(DynamicJsonDocument &dynamicJsonDocument)
{
    String messageType = String(dynamicJsonDocument["MessageType"]);

    if (messageType == "DeviceConnectedHardwareNotification")
    {
        IsConnected = true;
        return;
    }

    if (messageType == "DeviceCreatedHardwareNotification")
    {
        IsCreated = true;
        return;
    }

    if (messageType == "DeviceVerificationRequestedHardwareNotification")
    {
        IsVerificationStarted = true;
        return;
    }

    if (messageType == "ListDeviceAttachmentsResult")
    {
        IsAttachmentsListed = true;

        attachmentSlots = 0;
        int attachmentCount = int(dynamicJsonDocument["AttachmentCount"]);
        for (int i = 0; i < attachmentCount; i++)
        {
            String id = String(dynamicJsonDocument["Attachments"][i]["Id"]);
            String attachmentName = String(dynamicJsonDocument["Attachments"][i]["AttachmentName"]);
            String attachmentSerial = String(dynamicJsonDocument["Attachments"][i]["Serial"]);
            String capability = String(dynamicJsonDocument["Attachments"][i]["Capability"]);
            int powerPin = int(dynamicJsonDocument["Attachments"][i]["PowerPin"]);
            unsigned long measurementFrequency = (unsigned long)(dynamicJsonDocument["Attachments"][i]["MeasurementFrequency"]);

            attachments[i] = new DeviceAttachment(id, userId, deviceSerial, attachmentName, attachmentSerial, capability, powerPin, measurementFrequency);
            attachmentSlots++;
        }

        return;
    }

    if (messageType == "DeviceAttachmentCreatedHardwareNotification")
    {
        String deviceId = String(dynamicJsonDocument["DeviceId"]);
        if (this->deviceId != deviceId || this->attachmentSlots > 9)
        {
            Serial.println("Device id mismatch or slots full, DeviceId: " + this->deviceId + " Received deviceId: " + deviceId + ", Slots: 9/" + attachmentSlots);
            return;
        }

        Serial.println("Deserializing...");

        String id = String(dynamicJsonDocument["Id"]);
        String attachmentName = String(dynamicJsonDocument["AttachmentName"]);
        String attachmentSerial = String(dynamicJsonDocument["Serial"]);
        String capability = String(dynamicJsonDocument["Capability"]);
        int powerPin = int(dynamicJsonDocument["PowerPin"]);
        unsigned long measurementFrequency = (unsigned long)(dynamicJsonDocument["MeasurementFrequency"]);

        attachments[attachmentSlots] = new DeviceAttachment(id, userId, deviceSerial, attachmentName, attachmentSerial, capability, powerPin, measurementFrequency);

        Serial.println("Added to slot: " + String(attachmentSlots));

        attachmentSlots++;
        return;
    }

    if (messageType == "DeviceAttachmentRemovedHardwareNotification")
    {
        String id = String(dynamicJsonDocument["Id"]);

        // Todo: delete an attachment from the array of attachments

        return;
    }

    if (messageType == "DeviceAttachmentStateChangedHardwareNotification")
    {
        String id = String(dynamicJsonDocument["Id"]);

        for (int i = 0; i < attachmentSlots; i++)
        {
            if (attachments[i]->Id.equals(id))
            {
                Serial.println("Found: " + String(attachments[i]->Capability));

                String capability = attachments[i]->Capability;
                if (capability == "BinarySwitch")
                {
                    attachments[i]->Toggle(String(dynamicJsonDocument["State"]["Value"]) == "True");
                }
                else if (capability == "Dim")
                {
                    attachments[i]->Dim(float(dynamicJsonDocument["State"]["Value"]));
                }
            }
            else
            {
                Serial.println("Mismatch " + attachments[i]->Id + " vs " + id);
            }
        }

        return;
    }

    if (messageType == "ActivateDeviceAttachmentHardwareCommand")
    {
        // measure();
        return;
    }

    if (messageType == "DeviceVerificationResultHardwareNotification")
    {
        Serial.flush();

        IsVerified = true;
        String passphrase = String(dynamicJsonDocument["Passphrase"]);

        Serial.println("Got pass: " + passphrase);

        if (passphrase == "")
        {
            return;
        }

        struct
        {
            char pass[37] = "";
        } data;

        strcpy(data.pass, passphrase.c_str());

        SaveToEEPROM(&data, sizeof(data));

        return;
    }
}

void DeviceModel::TryCreate()
{
    if (!IsCreated)
    {
        Serial.println("Creating device");
        deviceService->CreateDevice(userId, deviceId, deviceSerial, deviceName);
    }
    else
    {
        Serial.println("Device already created");
    }
}

void DeviceModel::TryConnect()
{
    if (!IsConnected)
    {
        Serial.println("Connecting device");
        deviceService->ConnectDevice(userId, deviceId, deviceSerial);
    }
    else
    {
        Serial.println("Device already connected");
    }
}

void DeviceModel::TryVerifyFromRom()
{
    if (!IsCreated || !IsConnected || IsVerified)
    {
        return;
    }

    struct
    {
        char pass[37] = "";
    } data;

    LoadFromEEPROM(&data, sizeof(data));
    String passphrase = String(data.pass);

    Serial.println("Read pass from eeprom: " + passphrase);

    if (passphrase == "")
    {
        return;
    }

    deviceService->VerifyDevice(userId, deviceId, passphrase);
}

void DeviceModel::Verify(String &passphrase)
{
    deviceService->VerifyDevice(userId, deviceId, passphrase);
    IsVerificationStarted = false;
}

void DeviceModel::TryListDeviceAttachments()
{
    if (!IsAttachmentsListed && IsVerified)
    {
        Serial.println("Listing device attachments");
        deviceService->ListDeviceAttachments(userId, deviceId);
    }
}

void DeviceModel::measure()
{
    Serial.println("measure");
    String percent = "%";

    for (int i = 0; i < attachmentSlots; i++)
    {
        if (attachments[i]->ShouldMeasure())
        {
            float measurement = attachments[i]->Measure();
            deviceService->RecordMeasurement(userId, deviceId, attachments[i]->Id, measurement, percent);
            continue;
        }
    }
}