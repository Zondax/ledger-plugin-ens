import { processTest, populateTransaction } from "./test.fixture";

const contractName = "ETHRegistrar"; // <= Name of the smart contract

const testLabel = "eth_registrar_register_with_config"; // <= Name of the test
const testDirSuffix = "eth_registrar_register_with_config"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x283af0b28c62c092c9727f1ee09c02ca627eb7f5"; // <= Address of the smart contract
const chainID = 1;

// From : https://etherscan.io/tx/0x07528c73fe837a339e2f48188278e3f2fadabb8e56c7766ddadd69f3a009b0f5
const inputData = "0xf7a1696300000000000000000000000000000000000000000000000000000000000000c00000000000000000000000006e24b5365bb6dedb1e86b96124e25f5c6cd01bad0000000000000000000000000000000000000000000000000000000001e18558256bb9c92cf9b0ef511ce17c0401334a8238c521cb2e05e7939965b0465f33110000000000000000000000004976fb03c32e5b8cfe2b6ccb31c09ba78ebaba410000000000000000000000006e24b5365bb6dedb1e86b96124e25f5c6cd01bad000000000000000000000000000000000000000000000000000000000000000674757266696e0000000000000000000000000000000000000000000000000000";
// Create serializedTx and remove the "0x" prefix
const value = "3.1";
const serializedTx = populateTransaction(contractAddr, inputData, chainID, value);
const devices = [
   {
     name: "nanos",
     label: "Nano S",
     steps: 17, // <= Define the number of steps for this test case and this device
   },
  {
    name: "nanox",
    label: "Nano X",
    steps: 9, // <= Define the number of steps for this test case and this device
  },
  {
     name: "nanosp",
     label: "Nano S+",
     steps: 9, // <= Define the number of steps for this test case and this device
   },
];

devices.forEach((device) =>
  processTest(device, contractName, testLabel, testDirSuffix, "", signedPlugin, serializedTx, testNetwork)
);
