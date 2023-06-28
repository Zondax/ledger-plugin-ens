import { processTest, populateTransaction } from "./test.fixture";

const contractName = "ETHRegistrar"; // <= Name of the smart contract

const testLabel = "eth_registrar_register"; // <= Name of the test
const testDirSuffix = "eth_registrar_register"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x283af0b28c62c092c9727f1ee09c02ca627eb7f5"; // <= Address of the smart contract
const chainID = 1;

// From : https://etherscan.io/tx/0x6792413131e66b71890dbf6484c1a73df539c91fe83a367fc53e0eca8825254b
const inputData = "0x85f6d1550000000000000000000000000000000000000000000000000000000000000080000000000000000000000000d7b460f56aaf1458523e9ac8edfa551598490c2800000000000000000000000000000000000000000000000000000000003026ef6aaf1458523e9ac8edfa551598490c2800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000a3132333131313133313200000000000000000000000000000000000000000000";
// Create serializedTx and remove the "0x" prefix
const value = "3.1";
const serializedTx = populateTransaction(contractAddr, inputData, chainID, value);
const devices = [
   {
     name: "nanos",
     label: "Nano S",
     steps: 12, // <= Define the number of steps for this test case and this device
   },
  {
    name: "nanox",
    label: "Nano X",
    steps: 8, // <= Define the number of steps for this test case and this device
  },
  {
     name: "nanosp",
     label: "Nano S+",
     steps: 8, // <= Define the number of steps for this test case and this device
   },
];

devices.forEach((device) =>
  processTest(device, contractName, testLabel, testDirSuffix, "", signedPlugin, serializedTx, testNetwork)
);
