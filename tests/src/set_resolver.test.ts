import { processTest, populateTransaction } from "./test.fixture";

const contractName = "RegistryWithFallback"; // <= Name of the smart contract

const testLabel = "registry_fallback_set_resolver"; // <= Name of the test
const testDirSuffix = "registry_fallback_set_resolver"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x00000000000c2e074ec69a0dfb2997ba6c7d2e1e"; // <= Address of the smart contract
const chainID = 1;

// From : https://etherscan.io/tx/0x9a2146b6cacbb9130c07d93a5e3808e354df83ac19b82e974c333a4545c7bf1c
const inputData = "0x1896f70ad2e7e3664137da66f58286a519ba984c4f8ff45652c466d63b2464caec51ab58000000000000000000000000231b0ee14048e9dccd1d247744d114a4eb5e8e63";
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
