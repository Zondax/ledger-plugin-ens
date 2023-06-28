import { processTest, populateTransaction } from "./test.fixture";

const contractName = "ETHRegistrar"; // <= Name of the smart contract

const testLabel = "eth_registrar_commit"; // <= Name of the test
const testDirSuffix = "eth_registrar_commit"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x283af0b28c62c092c9727f1ee09c02ca627eb7f5"; // <= Address of the smart contract
const chainID = 1;

// From : https://etherscan.io/tx/0xea772f0f05543cc90e25a19997c0430d82e85331d45e2264603bc3cd2bbff434
const inputData = "0xf14fcbc86952f78134ad22871b951c2f5d1bab4f5f33359c35866dbbf2464993ee2b5589";
// Create serializedTx and remove the "0x" prefix
const value = "3.1";
const serializedTx = populateTransaction(contractAddr, inputData, chainID, value);
const devices = [
   {
     name: "nanos",
     label: "Nano S",
     steps: 7, // <= Define the number of steps for this test case and this device
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
