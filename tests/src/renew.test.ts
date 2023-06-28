import { processTest, populateTransaction } from "./test.fixture";

const contractName = "ETHRegistrar"; // <= Name of the smart contract

const testLabel = "eth_registrar_renew"; // <= Name of the test
const testDirSuffix = "eth_registrar_renew"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x283af0b28c62c092c9727f1ee09c02ca627eb7f5"; // <= Address of the smart contract
const chainID = 1;

// From : https://etherscan.io/tx/0x8b9cd8afc1067233b668e09c585bab4b46587ecc71018ee2aee4ec55472a3f0c
const inputData = "0xacf1a84100000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000001e1855800000000000000000000000000000000000000000000000000000000000000086d696c6f64696e6f000000000000000000000000000000000000000000000000";
// Create serializedTx and remove the "0x" prefix
const value = "3.1";
const serializedTx = populateTransaction(contractAddr, inputData, chainID, value);
const devices = [
   {
     name: "nanos",
     label: "Nano S",
     steps: 5, // <= Define the number of steps for this test case and this device
   },
  {
    name: "nanox",
    label: "Nano X",
    steps: 5, // <= Define the number of steps for this test case and this device
  },
  {
     name: "nanosp",
     label: "Nano S+",
     steps: 5, // <= Define the number of steps for this test case and this device
   },
];

devices.forEach((device) =>
  processTest(device, contractName, testLabel, testDirSuffix, "", signedPlugin, serializedTx, testNetwork)
);
