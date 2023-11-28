/*
 * SPDX-FileCopyrightText: Bosch Rexroth AG
 *
 * SPDX-License-Identifier: MIT
 */
#include <stdio.h>
#include <map>
#include <iostream>
#include <chrono>
#include <thread>
#include <csignal>

#include "comm/datalayer/datalayer.h"
#include "comm/datalayer/datalayer_system.h"
#include "comm/datalayer/memory_map_generated.h"

#include "ctrlx_datalayer_helper.h"

static bool g_endProcess = false;
static void signalHandler(int signal)
{
  std::cout << "signal: " << signal << std::endl;
  g_endProcess = true;
}

static void print(comm::datalayer::Variant * data)
{
  auto varientType = data->getType();
  if (varientType == comm::datalayer::VariantType::UINT8)
    {

      auto temp = uint8_t(*data);
      std::cout << "        : " << std::to_string(temp) << std::endl;
    }
}

int main(void)
{
  comm::datalayer::DlResult result;
  comm::datalayer::Variant data;
  comm::datalayer::Variant dataIn;
  comm::datalayer::DatalayerSystem datalayer;
  datalayer.start(false);

  comm::datalayer::IClient* client = getClient(datalayer);
  if (client == nullptr)
  {
    std::cout << "ERROR Client connected" << std::endl;
    datalayer.stop();
    return 1;
  }

  result = client->readSync("fieldbuses/ethercat/master/instances/VTEM/realtime_data/output/map", &data);
  std::cout << "Read returned: " << result.toString() << std::endl;

  result = client->readSync("fieldbuses/ethercat/master/instances/VTEM/realtime_data/input/map", &dataIn);
  std::cout << "Read returned: " << result.toString() << std::endl;

  std::signal(SIGINT, signalHandler);

  // First we have to open the realtimememory, reading the whole outputs.
  uint8_t* outData;
  std::cout << "Opening some realtime memory" << std::endl;
  std::shared_ptr<comm::datalayer::IMemoryUser> output;
  result = datalayer.factory()->openMemory(output, "fieldbuses/ethercat/master/instances/VTEM/realtime_data/output");
  std::cout << "Open the memory with: " << result.toString() << std::endl;
  if (comm::datalayer::STATUS_FAILED(result))
  {
    std::cout << "Open the memory failed with: " << result.toString() << std::endl;
  }

  // We can read the Inputs to get a Start trigger for example
  uint8_t* inData;
  std::cout << "Opening some realtime memory" << std::endl;
  std::shared_ptr<comm::datalayer::IMemoryUser> input;
  result = datalayer.factory()->openMemory(input, "fieldbuses/ethercat/master/instances/VTEM/realtime_data/input");
  std::cout << "Open the memory with: " << result.toString() << std::endl;
  if (comm::datalayer::STATUS_FAILED(result) && !g_endProcess)
  {
    std::cout << "Open the memory failed with: " << result.toString() << std::endl;
  }

  // for (int i = 0; i < 10; i++)
  // {
  //   std::this_thread::sleep_for(std::chrono::milliseconds(100));
  //   result = input->getMemoryMap(dataIn);
  //   std::cout << "Getting Input MemoryMap with: " << result.toString() << std::endl;
  // }
  // for (int i = 0; i < 10; i++)
  // {
  //   std::this_thread::sleep_for(std::chrono::milliseconds(100));
  //   result = output->getMemoryMap(data);
  //   std::cout << "Getting output MemoryMap with: " << result.toString() << std::endl;
  // }

  // At this point we can take the data. It is important to always check the revision number.
  // The revision number will be different if the layout will be changed.
  auto memMap = comm::datalayer::GetMemoryMap(data.getData());
  auto revision = memMap->revision();
  std::cout << "Output revision: " << revision << std::endl;
  std::string name;
  uint32_t offset = 0;
  std::cout << "Get Bitoffset and Name from Memory Map" << std::endl;

  // Iputs
  auto memMapInputs = comm::datalayer::GetMemoryMap(dataIn.getData());
  auto revisionIn = memMapInputs->revision();
  std::cout << "Input revision: " << revision << std::endl;
  std::string nameIn;
  uint32_t offsetIn = 0;
  std::cout << "Get Bitoffset and Name from Memory Map (Inputs) " << std::endl;

  // Save all the outputs (name and offset) into a map
  for (auto variable = memMap->variables()->begin(); variable != memMap->variables()->end(); variable++)
  {
    name = variable->name()->str();
    std::cout << "Name of Output we found: " << name << std::endl;
    offset = variable->bitoffset();
    std::cout << "Bitoffset: " << offset << std::endl;
  }

  std::string in_address = "fieldbuses/ethercat/master/instances/VTEM/realtime_data/input/data/";
  std::vector<std::string> in_addresses;
  // Save all the inputs (name and offset) into a map
  for (auto variable = memMapInputs->variables()->begin(); variable != memMapInputs->variables()->end(); variable++)
  {
    nameIn = variable->name()->str();
    std::cout << "Name of Input we found: " << nameIn << std::endl;
    offsetIn = variable->bitoffset();
    std::cout << "Bitoffset: " << offsetIn << std::endl;

    std::string add = in_address+nameIn;
    in_addresses.push_back(add);
  }

  for (int i = 0; i < in_addresses.size(); i++)
  {
    result = client->readSync(in_addresses[i], &dataIn);

    if (comm::datalayer::STATUS_FAILED(result))
    {
      std::cout << "Read sync: " << result.toString() << std::endl;
    }
    
    print(&dataIn);
  }

  comm::datalayer::Variant dataOut;
  dataOut.setType(comm::datalayer::VariantType::UINT8);
  dataOut.setValue(221);
  result = client->writeSync("fieldbuses/ethercat/master/instances/VTEM/realtime_data/output/data/CPX_FB37/VTEM_8_Valves_48DI_DO_/Outputs.Valve_0_Output_2", &dataOut);

  if (comm::datalayer::STATUS_FAILED(result))
    {
      std::cout << "Write sync: " << result.toString() << std::endl;
    }


  return 0;
}
