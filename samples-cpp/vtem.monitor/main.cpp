#include "comm/datalayer/datalayer.h"
#include "comm/datalayer/datalayer_system.h"
#include "comm/datalayer/memory_map_generated.h"
#include <stdio.h>
#include <csignal>
#include <thread>
#include <iostream>

// Create end process flag witch is set to true if SIGINT is send
static bool g_endProcess = false;
static void signalHandler(int signal)
{
  std::cout << "signal: " << signal << std::endl;
  g_endProcess = true;
}

static void closeUserMemory(comm::datalayer::DatalayerSystem* datalayer,
                            std::shared_ptr<comm::datalayer::IMemoryUser> userMemory)
{
  if (userMemory == nullptr)
  {
    return;
  }

  std::cout << "INFO Closing realtime user memory" << std::endl;

  comm::datalayer::DlResult result = datalayer->factory()->closeMemory(userMemory);
  if (comm::datalayer::STATUS_FAILED(result))
  {
    std::cout << "WARNING Closing realtime user memory failed with: " << result.toString() << std::endl;
  }
}
// Cleanup closes the memory and stop the datalayersystem
static void cleanup(comm::datalayer::DatalayerSystem* datalayer,
                    std::shared_ptr<comm::datalayer::IMemoryUser> input,
                    std::shared_ptr<comm::datalayer::IMemoryUser> output)
{
  closeUserMemory(datalayer, input);
  closeUserMemory(datalayer, output);

  datalayer->stop();
}

int main()
{
  comm::datalayer::DlResult result;
  comm::datalayer::Variant dataOut;
  comm::datalayer::Variant dataIn;
  comm::datalayer::DatalayerSystem datalayer;
  uint8_t* inData;
  uint8_t* outData;

  datalayer.start(false);
  std::cout << "INFO Datalayer started" << std::endl;
  std::shared_ptr<comm::datalayer::IMemoryUser> input;
  result = datalayer.factory()->openMemory(input, "fieldbuses/ethercat/master/instances/VTEM/realtime_data/input");
  if (comm::datalayer::STATUS_FAILED(result))
  {
    std::cout << "ERROR Opening realtime memory sdk-cpp-realtime/rt/input failed with: " << result.toString() << std::endl;
    return 1;
  }

  std::shared_ptr<comm::datalayer::IMemoryUser> output;
  result = datalayer.factory()->openMemory(output, "fieldbuses/ethercat/master/instances/VTEM/realtime_data/output");
  if (comm::datalayer::STATUS_FAILED(result))
  {
    std::cout << "ERROR Opening realtime memory sdk-cpp-realtime/rt/output failed with: " << result.toString() << std::endl;
    cleanup(&datalayer, input, nullptr);
    return 1;
  }

  std::cout << "INFO Get memory map input" << std::endl;
  result = input->getMemoryMap(dataIn);
  if(comm::datalayer::STATUS_FAILED(result))
  {
    std::cout << "ERROR Get memory map failed with: " << result.toString() << std::endl;
    cleanup(&datalayer, input, output);
    return 1;
  }

  std::cout << "INFO Get memory map output" << std::endl;
  result = output->getMemoryMap(dataOut);
  if(comm::datalayer::STATUS_FAILED(result))
  {
    std::cout << "ERROR Get memory map failed with: " << result.toString() << std::endl;
    cleanup(&datalayer, input, output);
    return 1;
  }

  result = dataIn.verifyFlatbuffers(comm::datalayer::VerifyMemoryMapBuffer);  // Verify the memory map
  if(comm::datalayer::STATUS_FAILED(result))
  {
    std::cout << "ERROR Verify memory map failed with: " << result.toString() << std::endl;
    cleanup(&datalayer, input, output);
    return 1;
  }

  result = dataOut.verifyFlatbuffers(comm::datalayer::VerifyMemoryMapBuffer);  // Verify the memory map
  if(comm::datalayer::STATUS_FAILED(result))
  {
    std::cout << "ERROR Verify memory map failed with: " << result.toString() << std::endl;
    cleanup(&datalayer, input, output);
    return 1;
  }

  auto memMapOutputs = comm::datalayer::GetMemoryMap(dataOut.getData());
  auto revisionOut = memMapOutputs->revision();
  std::string name;
  uint32_t offset = 0;
  std::map<std::string, uint32_t> mapOfOutputs;
  std::cout << "Get Bitoffset and Name from Memory Map" << std::endl;

  // Iputs
  auto memMapInputs = comm::datalayer::GetMemoryMap(dataIn.getData());
  auto revisionIn = memMapInputs->revision();
  std::string nameIn;
  uint32_t offsetIn = 0;
  std::map<std::string, uint32_t> mapOfInputs;
  std::cout << "Get Bitoffset and Name from Memory Map (Inputs) " << std::endl;


  for(auto variable = memMapInputs->variables()->begin(); variable != memMapInputs->variables()->end(); variable++)
  {
    std::cout << "Name of Input we found: " << variable->name()->str() << " value: " << variable->bitoffset() << std::endl;
  }

  for(auto variable = memMapOutputs->variables()->begin(); variable != memMapOutputs->variables()->end(); variable++)
  {
    std::cout << "Name of Output we found: " << variable->name()->str() << " value: " << variable->bitoffset() << std::endl;
  }

  std::signal(SIGINT, signalHandler);
  return 0;
}
