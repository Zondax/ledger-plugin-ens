import { processTest, populateTransaction } from "./test.fixture";

const contractName = "PublicResolver"; // <= Name of the smart contract

const testLabel = "eth_resolver_set_text"; // <= Name of the test
const testDirSuffix = "eth_resolver_set_text"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x231b0ee14048e9dccd1d247744d114a4eb5e8e63"; // <= Address of the smart contract
const chainID = 1;

// From : https://etherscan.io/tx/0x7ac068b74aa0bf96ba03060b84f8d0decd9fe4e8069e3fe489ec7c7022a3997f
const inputData = "0x10f13a8c4520c6739ae1af5764d371af2ffb0596f362158b65573c77c2d4ac9e40ce9b46000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000066176617461720000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001f68747470733a2f2f692e696d6775722e636f6d2f63624b763431322e6a706700";
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
