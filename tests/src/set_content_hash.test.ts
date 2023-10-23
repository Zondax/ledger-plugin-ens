import { processTest, populateTransaction } from "./test.fixture";

const contractName = "PublicResolver"; // <= Name of the smart contract

const testLabel = "eth_resolver_set_content_hash"; // <= Name of the test
const testDirSuffix = "eth_resolver_set_content_hash"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x231b0ee14048e9dccd1d247744d114a4eb5e8e63"; // <= Address of the smart contract
const chainID = 1;

// From : https://etherscan.io/tx/0x9fc4d634107182767420f129c79737ddd232b5efd64f017c2b58f9495baa1290
const inputData = "0x304e6adea85b9975f0587b985e4b5b1f44cff69b583caaa6a3c8bb45a3ff890fcfc13b5e0000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000002ae50101720024080112201840f1be75cdddc5e348868b570accf86a742e5f4cc3fa435e54dcef4fce3b9d00000000000000000000000000000000000000000000";
// Create serializedTx and remove the "0x" prefix
const value = "3.1";
const serializedTx = populateTransaction(contractAddr, inputData, chainID, value);
const devices = [
   {
     name: "nanos",
     label: "Nano S",
     steps: 11, // <= Define the number of steps for this test case and this device
   },
  {
    name: "nanox",
    label: "Nano X",
    steps: 7, // <= Define the number of steps for this test case and this device
  },
  {
     name: "nanosp",
     label: "Nano S+",
     steps: 7, // <= Define the number of steps for this test case and this device
   },
];

devices.forEach((device) =>
  processTest(device, contractName, testLabel, testDirSuffix, "", signedPlugin, serializedTx, testNetwork)
);
