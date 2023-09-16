// DeviceModel.cpp
#include "DeviceModel.h"

#include <Arduino.h>
#include <ArduinoJson.h>
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
        // TODO: Start device verification flow
        return;
    }

    if (messageType == "ListDeviceAttachmentsResult")
    {
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
        String ownerDeviceId = String(dynamicJsonDocument["OwnerDeviceId"]);
        if (ownerDeviceId != deviceId || attachmentSlots > 9)
        {
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
        measure();
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
        }
    }
}