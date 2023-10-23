import { processTest, populateTransaction } from "./test.fixture";

const contractName = "ReverseRegistrar"; // <= Name of the smart contract

const testLabel = "reverse_registrar_set_name_66bytes"; // <= Name of the test
const testDirSuffix = "reverse_registrar_set_name_66bytes"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x084b1c3c81545d370f3634392de611caabff8148"; // <= Address of the smart contract
const chainID = 1;

// From : https://etherscan.io/tx/0x905e106763556d0cb16d1fc11ab13d75cf5b1227e0480098b909bba50c4271b8
const inputData = "0xc47f002700000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000042414141414141414141414141414141414141414141414141414141414141414142424242424242424242424242424242424242424242424242424242424242424343000000000000000000000000000000000000000000000000000000000000";
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
    steps: 4, // <= Define the number of steps for this test case and this device
  },
  {
     name: "nanosp",
     label: "Nano S+",
     steps: 4 // <= Define the number of steps for this test case and this device
   },
];

devices.forEach((device) =>
  processTest(device, contractName, testLabel, testDirSuffix, "", signedPlugin, serializedTx, testNetwork)
);
