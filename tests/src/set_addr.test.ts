import { processTest, populateTransaction } from "./test.fixture";

const contractName = "PublicResolver"; // <= Name of the smart contract

const testLabel = "eth_resolver_set_addr"; // <= Name of the test
const testDirSuffix = "eth_resolver_set_addr"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x231b0ee14048e9dccd1d247744d114a4eb5e8e63"; // <= Address of the smart contract
const chainID = 1;

// From : https://etherscan.io/tx/0xb6bc792ffc109a486b0f3ecc918d83f2cec3f1d12a57c5a1e680b59dc84b9217
const inputData = "0x8b95dd71258b3bf5eab9cfd4fc2df1e34716fd22c441698464e6f8716786d1f47e54242b000000000000000000000000000000000000000000000000000000000000003c0000000000000000000000000000000000000000000000000000000000000060000000000000000000000000000000000000000000000000000000000000001409bce690af4a5d45cb035489c436313edaf02af9000000000000000000000000";
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
