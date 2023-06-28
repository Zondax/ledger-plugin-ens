import { processTest, populateTransaction } from "./test.fixture";

const contractName = "RegistryWithFallback"; // <= Name of the smart contract

const testLabel = "registry_fallback_set_subnode_resolver"; // <= Name of the test
const testDirSuffix = "registry_fallback_set_subnode_resolver"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x00000000000c2e074ec69a0dfb2997ba6c7d2e1e"; // <= Address of the smart contract
const chainID = 1;

// From : https://etherscan.io/tx/0xdf3fbe0737bdcbc3d8a88a5f68b4b9243e56ba0bad527b6911ce58867f378c3d
const inputData = "0x5ef2c7f0b80478f266d748625c0d3208f7ded727374ab3f3a4f30c230e6dbbb62f462b5afd341581598f70c2162381faf4328118039b3b0e71e7cd41ea4b3eb2411e9d330000000000000000000000004e903fb3c9e37ab1ed595172781a11859b52c4c80000000000000000000000004976fb03c32e5b8cfe2b6ccb31c09ba78ebaba410000000000000000000000000000000000000000000000000000000000000000";
// Create serializedTx and remove the "0x" prefix
const value = "3.1";
const serializedTx = populateTransaction(contractAddr, inputData, chainID, value);
const devices = [
   {
     name: "nanos",
     label: "Nano S",
     steps: 18, // <= Define the number of steps for this test case and this device
   },
  {
    name: "nanox",
    label: "Nano X",
    steps: 10, // <= Define the number of steps for this test case and this device
  },
  {
     name: "nanosp",
     label: "Nano S+",
     steps: 10 // <= Define the number of steps for this test case and this device
   },
];

devices.forEach((device) =>
  processTest(device, contractName, testLabel, testDirSuffix, "", signedPlugin, serializedTx, testNetwork)
);
