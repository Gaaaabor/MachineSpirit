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

    if (messageType == "ListDeviceAttachmentsHardwareQueryResult")
    {
        IsAttachmentsListed = true;

        attachmentSlots = 0;
        int attachmentCount = int(dynamicJsonDocument["DeviceAttachmentCount"]);       

        for (int i = 0; i < attachmentCount; i++)
        {
            String id = String(dynamicJsonDocument["DeviceAttachments"][i]["Id"]);
            String name = String(dynamicJsonDocument["DeviceAttachments"][i]["Name"]);
            String serial = String(dynamicJsonDocument["DeviceAttachments"][i]["Serial"]);
            String capability = String(dynamicJsonDocument["DeviceAttachments"][i]["Capability"]);
            int powerPin = int(dynamicJsonDocument["DeviceAttachments"][i]["PowerPin"]);
            unsigned long measurementFrequency = (unsigned long)(dynamicJsonDocument["DeviceAttachments"][i]["MeasurementFrequency"]);

            deviceAttachments[i] = new DeviceAttachment(id, userId, deviceSerial, name, serial, capability, powerPin, measurementFrequency);            

            attachmentSlots++;

            if (capability == "Switch")
            {
                String state = String(dynamicJsonDocument["DeviceAttachments"][i]["State"]["Value"]);
                if (state == "True")
                {
                    deviceAttachments[i]->Switch(true);
                }
                else
                {
                    deviceAttachments[i]->Switch(false);
                }
            }
        }

        return;
    }

    if (messageType == "DeviceAttachmentCreatedHardwareNotification")
    {
        String deviceId = String(dynamicJsonDocument["DeviceId"]);
        if (this->deviceId != deviceId || this->attachmentSlots > 9)
        {
            return;
        }

        String id = String(dynamicJsonDocument["Id"]);
        String attachmentName = String(dynamicJsonDocument["Name"]);
        String attachmentSerial = String(dynamicJsonDocument["Serial"]);
        String capability = String(dynamicJsonDocument["Capability"]);
        int powerPin = int(dynamicJsonDocument["PowerPin"]);
        unsigned long measurementFrequency = (unsigned long)(dynamicJsonDocument["MeasurementFrequency"]);

        deviceAttachments[attachmentSlots] = new DeviceAttachment(id, userId, deviceSerial, attachmentName, attachmentSerial, capability, powerPin, measurementFrequency);

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
            if (deviceAttachments[i]->Id.equals(id))
            {
                String capability = deviceAttachments[i]->Capability;
                if (capability == "Switch")
                {
                    deviceAttachments[i]->Switch(String(dynamicJsonDocument["State"]["Value"]) == "True");
                }
                else if (capability == "Dim")
                {
                    deviceAttachments[i]->Range(float(dynamicJsonDocument["State"]["Value"]));
                }
            }
        }

        return;
    }

    if (messageType == "ActivateDeviceAttachmentHardwareCommand")
    {
        String deviceAttachmentId = String(dynamicJsonDocument["DeviceAttachmentId"]);
        String percent = "%";
        String requestId = String(dynamicJsonDocument["RequestId"]);

        for (int i = 0; i < attachmentSlots; i++)
        {
            if (deviceAttachments[i]->Id == deviceAttachmentId)
            {
                if (deviceAttachments[i]->Capability == "Measure")
                {
                    float measurement = deviceAttachments[i]->Measure();
                    deviceService->RecordMeasurement(userId, deviceId, deviceAttachments[i]->Id, measurement, percent, requestId);
                    continue;
                }

                if (deviceAttachments[i]->Capability == "Switch")
                {
                    bool triggeredValue = dynamicJsonDocument["TriggeredSwitchValue"].as<bool>();
                    bool measurement = deviceAttachments[i]->Switch(triggeredValue);
                    deviceService->RecordSwitch(userId, deviceId, deviceAttachments[i]->Id, measurement, requestId);
                    continue;
                }

                if (deviceAttachments[i]->Capability == "Range")
                {
                    float triggeredValue = dynamicJsonDocument["TriggeredRangeValue"].as<float>();
                    float measurement = deviceAttachments[i]->Range(triggeredValue);
                    deviceService->RecordRange(userId, deviceId, deviceAttachments[i]->Id, measurement, requestId);
                    continue;
                }
            }
        }
        return;
    }

    if (messageType == "DeviceVerificationResultHardwareNotification")
    {
        Serial.flush();

        IsVerified = true;
        String passphrase = String(dynamicJsonDocument["Passphrase"]);

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
    else
    {
        Serial.println("Device attachments already listed");        
    }
}

void DeviceModel::measure()
{
    Serial.println("measure");
    String percent = "%";
    String requestId = "{E7319AB6-24FB-41A2-8302-9D162F3437D3}"; // Nothing special, just wanted to fill with something

    for (int i = 0; i < attachmentSlots; i++)
    {
        if (deviceAttachments[i]->ShouldMeasure())
        {
            float measurement = deviceAttachments[i]->Measure();
            deviceService->RecordMeasurement(userId, deviceId, deviceAttachments[i]->Id, measurement, percent, requestId);
            continue;
        }
    }
}