import { processTest, populateTransaction } from "./test.fixture";

const contractName = "BulkRenewal"; // <= Name of the smart contract

const testLabel = "bulk_renewal_renew_all"; // <= Name of the test
const testDirSuffix = "bulk_renewal_renew_all"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0xff252725f6122a92551a5fa9a6b6bf10eb0be035"; // <= Address of the smart contract
const chainID = 1;

// From : https://etherscan.io/tx/0x5934824da5762b2661caaa3ec45fb102a8d6589c680c9da7f21bccb6b3b8246c
const inputData = "0xe8d6dbb4000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000003026ef0000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000003347477000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000047174776f00000000000000000000000000000000000000000000000000000000";
// Create serializedTx and remove the "0x" prefix
const value = "3.1";
const serializedTx = populateTransaction(contractAddr, inputData, chainID, value);
const devices = [
   {
     name: "nanos",
     label: "Nano S",
     steps: 6, // <= Define the number of steps for this test case and this device
   },
  {
    name: "nanox",
    label: "Nano X",
    steps: 6, // <= Define the number of steps for this test case and this device
  },
  {
     name: "nanosp",
     label: "Nano S+",
     steps: 6, // <= Define the number of steps for this test case and this device
   },
];

devices.forEach((device) =>
  processTest(device, contractName, testLabel, testDirSuffix, "", signedPlugin, serializedTx, testNetwork)
);
