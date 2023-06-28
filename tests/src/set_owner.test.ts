import { processTest, populateTransaction } from "./test.fixture";

const contractName = "RegistryWithFallback"; // <= Name of the smart contract

const testLabel = "registry_fallback_set_owner"; // <= Name of the test
const testDirSuffix = "registry_fallback_set_owner"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x00000000000c2e074ec69a0dfb2997ba6c7d2e1e"; // <= Address of the smart contract
const chainID = 1;

// From : https://etherscan.io/tx/0x7a6335c07bfc09c960fb3ed774c019a511c2011ade1408c48fa8502b4403230c
const inputData = "0x5b0fc9c3f70f0976117fe1a364173f15e56c723e15e4d1def7e882d68ff5126b53305f890000000000000000000000001a7006e11fe389629fbc24c75a90ca2979e798dd";
// Create serializedTx and remove the "0x" prefix
const value = "3.1";
const serializedTx = populateTransaction(contractAddr, inputData, chainID, value);
const devices = [
   {
     name: "nanos",
     label: "Nano S",
     steps: 10, // <= Define the number of steps for this test case and this device
   },
  {
    name: "nanox",
    label: "Nano X",
    steps: 6, // <= Define the number of steps for this test case and this device
  },
  {
     name: "nanosp",
     label: "Nano S+",
     steps: 6 // <= Define the number of steps for this test case and this device
   },
];

devices.forEach((device) =>
  processTest(device, contractName, testLabel, testDirSuffix, "", signedPlugin, serializedTx, testNetwork)
);
