/*
 * SPDX-FileCopyrightText: Bosch Rexroth AG
 *
 * SPDX-License-Identifier: MIT
 */

 /*
  * This snap-program determines symbolic PLC variables from plc-program 'PLC_PRG' and writes in all variables of
  * the type string 'HelloDeveloper', in all integers '1337' and in all floats the value '0.815f'.
  */

#include <stdio.h>
#include <iostream>
#include <string_view>

#include "comm/datalayer/datalayer.h"
#include "comm/datalayer/datalayer_system.h"

#include "ctrlx_datalayer_helper.h"

comm::datalayer::IClient* client;
comm::datalayer::Variant variant;

std::string helloPlcApplication = "hello_plc_application";


static comm::datalayer::DlResult getStrings(comm::datalayer::Variant& data, std::vector<std::string>& strings)
{
  if (data.getType() != comm::datalayer::VariantType::ARRAY_OF_STRING)
  {
    return comm::datalayer::DlResult::DL_RT_NOVALIDDATA;
  }

  strings.clear();

  const char** str = data;
  for (uint32_t i = 0; i < data.getCount(); i++)
  {
    std::cout << str[i] << std::endl;
    strings.push_back(str[i]);
  }
  return comm::datalayer::DlResult::DL_OK;
}

// Function takes the value from "i" to "data" and
// then writes the new "data" to the address "addr" of the ctrlX Data Layer client "IClient"
static comm::datalayer::DlResult setIntValue(comm::datalayer::Variant& data, comm::datalayer::IClient* client, std::string addr, int16_t i)
{
  comm::datalayer::DlResult result;
  result = data.setValue(i);
  if (STATUS_SUCCEEDED(result))
  {
    result = client->writeSync(addr, &data);
    if (STATUS_SUCCEEDED(result))
    {
      return comm::datalayer::DlResult::DL_OK;
    }
  }

  std::cout << "setting int value failed with: " << result.toString() << std::endl;
  return comm::datalayer::DlResult::DL_FAILED;
}

// Function takes the value from "str" to "data" and
// then writes the new "data" to the address "addr" of the ctrlX Data Layer client "IClient"
static comm::datalayer::DlResult setStringValue(comm::datalayer::Variant& data, comm::datalayer::IClient* client, std::string addr, std::string str)
{
  comm::datalayer::DlResult result;
  result = data.setValue(str);
  if (STATUS_SUCCEEDED(result))
  {
    result = client->writeSync(addr, &data);
    if (STATUS_SUCCEEDED(result))
    {
      return comm::datalayer::DlResult::DL_OK;
    }
  }

  std::cout << "setting string value failed with: " << result.toString() << std::endl;
  return comm::datalayer::DlResult::DL_FAILED;
}

// Function takes the value from "value" to "data" and
// then writes the new "data" to the address "addr" of the ctrlX Data Layer client "IClient"
static comm::datalayer::DlResult setFloatValue(comm::datalayer::Variant& data, comm::datalayer::IClient* client, std::string addr, float value)
{
  comm::datalayer::DlResult result;
  result = data.setValue(value);
  if (STATUS_SUCCEEDED(result))
  {
    result = client->writeSync(addr, &data);
    if (STATUS_SUCCEEDED(result))
    {
      return comm::datalayer::DlResult::DL_OK;
    }
  }

  std::cout << "setting value failed with: " << result.toString() << std::endl;
  return comm::datalayer::DlResult::DL_FAILED;
}

static void scanVariable(std::string address)
{
  auto writeNotAllowed = address.find(helloPlcApplication) == std::string::npos;

  comm::datalayer::DlResult result;

  // determine the data type of the symbol-variable read
  switch (variant.getType())
  {
    case comm::datalayer::VariantType::STRING:

      std::cout << "Actual value of '" << address << "' : " << std::string(variant) << std::endl;

      if (writeNotAllowed) 
      {
        std::cout << "Skip writing of '" << address << std::endl;
        return;
      }
      
      result = setStringValue(variant, client, address, "HelloDeveloper");
      if (STATUS_FAILED(result))
      {
        std::cout << "WARN Set STRING value into '" << address << "' failed with: " << result.toString() << std::endl;
      }
      else
      {
        std::cout << "INFO Set STRING value into '" << address << "' succeeded" << std::endl;
      }
      break;

      // Set '1337' to INT-Variable in the plc application
    case comm::datalayer::VariantType::INT16:

      std::cout << "Actual value of '" << address << "' : " << int16_t(variant) << std::endl;

      if (writeNotAllowed) 
      {
        std::cout << "Skip writing of '" << address << std::endl;
        return;
      }

      result = setIntValue(variant, client, address, 1337);
      if (STATUS_FAILED(result))
      {
        std::cout << "WARN Set INT16 value into '" << address << "' failed with: " << result.toString() << std::endl;
      }
      else
      {
        std::cout << "INFO Set INT16 value into '" << address << "' succeeded" << std::endl;
      }
      break;;

      // Set '0.815f' to Float-Variable in the plc application
    case comm::datalayer::VariantType::FLOAT32:

      std::cout << "Actual value of '" << address << "' : " << float(variant) << std::endl;

      if (writeNotAllowed) 
      {
        std::cout << "Skip writing of '" << address << std::endl;
        return;
      }

      result = setFloatValue(variant, client, address, 0.815f);
      if (STATUS_FAILED(result))
      {
        std::cout << "WARN Set FLOAT32 value into '" << address << "' failed with: " << result.toString() << std::endl;
      }
      else
      {
        std::cout << "INFO Set FLOAT32 value into '" << address << "' succeeded" << std::endl;
      }
      break;

    default:
      std::cout << "WARN Variant type of '" << address << "' not supported  - only STRING, INT16 or FLOAT32" << std::endl;
      break;
  }
}

// address without / at the end
static int scanFolder(std::string address)
{
  comm::datalayer::DlResult result = client->browseSync(address, &variant);
  if (STATUS_FAILED(result))
  {
    std::cout << "ERROR Browsing of '" << address << "' failed with: " << result.toString() << std::endl;
    return 1;
  }

  std::vector<std::string> children;
  result = getStrings(variant, children);
  if (STATUS_FAILED(result))
  {
    std::cout << "ERROR Reading '" << address << "' failed with: " << result.toString() << std::endl;
    return 1;
  }

  if (children.size() <= 0)
  {
    std::cout << "ERROR '" << address << "' has no sub elements" << std::endl;
    return 2;
  }

  // Loop over all children
  int exitCode;
  for (int i=0; i < children.size(); i++)
  {
    std::string childAddress = address +"/" + children[i];

    // If readSync succeeds then we have a variable
    result = client->readSync(childAddress, &variant);
    if (STATUS_SUCCEEDED(result))
    {
      scanVariable(childAddress);
      continue;
    }
    
    exitCode = scanFolder(childAddress);

    if (exitCode != 0) return exitCode;
  }

  return 0;
}
static int scanPlcApp()
{
  std::string address = "plc/app";
  comm::datalayer::DlResult result = client->browseSync(address, &variant);
  if (STATUS_FAILED(result))
  {
    std::cout << "ERROR Browsing of '" << address << "' failed with: " << result.toString() << std::endl;
    return 1;
  }

  std::vector<std::string> apps;
  result = getStrings(variant, apps);
  if (STATUS_FAILED(result))
  {
    std::cout << "ERROR Reading '" << address << "' failed with: " << result.toString() << std::endl;
    return 2;
  }

  if (apps.size() <= 0)
  {
    std::cout << "ERROR '" << address << "' has no sub elements" << std::endl;
    return 3;
  }

  // Loop over all apps
  for (int i=0; i < apps.size(); i++)
  {
    std::string symAddress = address +"/" + apps[i] + "/sym";
    auto result = scanFolder(symAddress);
    if (result > 0) return result;
  }

  return 0;
}

int main()
{
  int exitCode = 0;
  comm::datalayer::DatalayerSystem datalayer;
  datalayer.start(false);

  // Try ctrlX CORE (virtual)
  client = getClient(datalayer);
  if (client == nullptr)
  {
    // Try ctrlX COREvirtual with port forwarding
    client = getClient(datalayer, "10.0.2.2", "boschrexroth", "boschrexroth", 8443);
  }
  if (client == nullptr)
  {
    std::cout << "ERROR Creating client connection failed." << std::endl;
    exitCode = 1;
  }
  else
  {
    exitCode = scanPlcApp();
    delete client;
  }

  datalayer.stop();

  return exitCode;
}
