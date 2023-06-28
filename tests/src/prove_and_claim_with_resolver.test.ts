import { processTest, populateTransaction } from "./test.fixture";

const contractName = "DNSRegistrar"; // <= Name of the smart contract

const testLabel = "dns_registrar_prove_claim_with_resolver"; // <= Name of the test
const testDirSuffix = "dns_registrar_prove_claim_with_resolver"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x58774bb8acd458a640af0b88238369a167546ef2"; // <= Address of the smart contract
const chainID = 1;

// From : https://etherscan.io/tx/0x39544a43c7be74d230e606194444401995de781b095ca5ec167554ab70303841
const inputData = 
  "0x224199c200000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000e00000000000000000000000000000000000000000000000000000000000000b80000000000000000000000000231b0ee14048e9dccd1d247744d114a4eb5e8e6300000000000000000000000064233eaa064ef0d54ff1a963933d0d2d46ab582900000000000000000000000000000000000000000000000000000000000000090365746803766574000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000500000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000280000000000000000000000000000000000000000000000000000000000000066000000000000000000000000000000000000000000000000000000000000007c00000000000000000000000000000000000000000000000000000000000000960000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c00000000000000000000000000000000000000000000000000000000000000046002b080100015180649721106485ef80ee1b000376657400002b00010001518000244d9808022b1b3ab9f4fdd837fc9f9c7875fa916b6a22c3ef5c0c04d5d3235e42828cf6a8000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001009fd4c7311f33fd8621e714c300532689782a3cf29f701fc8e9469caad781891ab3713b8fbfdfd2d80f7fda1233f4c5b880d939ea54ccbca53ca3da5d3bc4fa1c5fbf719ef1edcf99119bee4def99ce90b04a18619b15af4dda4d81e8a022de8d264c15497ade1e0d9d4b17b3d1c9905e27d1ae9b0898f3c5984e8a34c09c56373cf12ec41f17c75c26c594c0b9913d8a4f38169f0cf56f6c039ad1a28bd741e25de95f15ac0b98521bf8ed9ab37770ce9c376085e6ded48eed859d46f182a1427443f11c9254de7a09db1af2163722157c2eab3af896f3bca2b60be9fb0132dd75c495a4448b7c4996e6feb4b4dbe599b5b358ee5245ae6bf2f99700511e8213000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000002c0000000000000000000000000000000000000000000000000000000000000025c0030080100000e10649ef9fa64833c6a4d98037665740003766574000030000100000e1000880100030803010001b84c7e78be7457690990b11b581921f725de0c2cc52d391f9b8ff4b5926022f1b2e9f7e532d2a29ad475a461d2b1aa556ada234d74f0ed1983cb277a2dc2ecab23a1708d739a2bbc744af1de6c0ba602f94ac2c74868aefae21753d135a108a216cae8543b6f0b7a5ab19cdebf60dbc6a155800c13678a658c156dba35a3bb4703766574000030000100000e1000880100030803010001e2e7c6545efc1d34aa24131591ce76e4d6fd85ad78d2e336bcef311e1269e48705e69aca35a49ed1ef53f54d0c6ba1bc40bbac061d5443fa9acab735a927d2c8b869f295c88fdead0f5b6b60f5f96365e98b42cd154cf79e2c4995e491c8061332313aa10ec00cab347b20a0e123e204f751f17a8355097e9c73442d2d060c6903766574000030000100000e10010801010308030100019b12ce71ef2072472a3e24255e0c8d31652e1f44d689f40c6c01dce590c9bdb9528772c67935665fdccf7b4dca532fbd64a546195b3db625a44c1c74b3b946deee2fa99c5f60e90e3ce246bfff7b190a11987b903d772739e5b9fe8a47cdfb85283fcc459ba6048aae933b149c6f71e6ef20951894ee78a099589389999bece4d68f51198303cc347b7bdc94b3c012b43c294a85417db35926963087b0af4e44ba7cdd86a0d8e3a7f716fffad7fa7821b9ecde9207d1c26c159ce8b1730c47bcffc8da7e1462236fba5a7733ddd7e2eeafbdbba51db29eb8bfda71f43862ed47a5f69bdde7fc2dd4af9bfd89706f7b7519e27a93e76a4246acc2257fb04ae9af00000000000000000000000000000000000000000000000000000000000000000000010050366659d26c4d66047606fc541a46c68f234c1ea8f5bd8ab1c854e9f1027d55cfce1e61d3695986a9b8c50c65426c66e9e788c70a8e71efd0d4ef5a2ebee05fd1cecffe97d1842c0b358b6f7f361ce16c31734dc6b9823f57dfd140c4094b30567d985bf8c66f35252ea7183b5e18bb3fe0e139af0c0fba586dff5a45460a3831cd18daa190387bdee1ad3da819c3557928a292db39fb764c1838b46df1b9b9c7595f9499c949ed9fc8b2b3d0fe0ef391fd64ebac7a92c02288569e86f4402c147e6b0f48e7e50230281012919f818c9a056fea25f5cd01fd386bf8cb221d17de44d0623e760ca9515b117f3647160b2e05fd39b53dac8e271341182172df3a000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000004e002b080200000e1064a21e9a6486610a9de00376657400036574680376657400002b000100000e10002409430d0279469d137d15392fbb74625b3d11b77c257da741cbc654c969bdd3f33e6ab4620000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000080ad14745a6b3eaa8b0c511a9ef3c3923349fcc2f4c01251ce1c54a2252d5856044a2bfa470ba06fb17b00224cde93f12110c1a4818e25bda60c2c53a4109f3c34b87a6f09a04af1a922d7347d3d20becc3078ec359011cdb636aceee16f021ab4435fc982abdbf2cbc11552dc5bb812af86e621f5d72f90efbd66767bdf99e9400000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000014000000000000000000000000000000000000000000000000000000000000000c900300d0200000e1064d5873264851bb209430365746803766574000365746803766574000030000100000e1000440100030da09311112cf9138818cd2feae970ebbd4d6a30f6088c25b325a39abbc5cd1197aa098283e5aaf421177c2aa5d714992a9957d1bcc18f98cd71f1f1806b65e1480365746803766574000030000100000e1000440101030d99db2cc14cabdc33d6d77da63a2f15f71112584f234e8d1dc428e39e8a4a97e1aa271a555dc90701e17e2a4c4b6f120b7c32d44f4ac02bd894cf2d4be7778a19000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000406ac61847d429e12e370891aebb1bf7cac606ed98ae7c128e4ac1189abbf447168a8ee8286c6de719493aa6fc07df318e73a85df8eac7c8259a75f8bc1c6da9d7000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000006000100d030000012c6487d1796485125986c9036574680376657400045f656e73036574680376657400001000010000012c002d2c613d30783634323333654161303634656630643534666631413936333933334430443264343661623538323900000000000000000000000000000000000000000000000000000000000000409408c381102d9866915ab372157a800a9a35350f4b2db6bd3a5f1c3798546239b93902c25c64b8c3ca72cbabcd00969e5b932edbdbf479184b182e2676fb8f5e000000000000000000000000000000000000000000000000000000000000022600003000010002a30001080100030803010001b1752c0c4440fb42944427a38f24eaeec98c239f55fc1d10d3b1414ad7d096e93790e72863002887dd8966b9a0378a4373a8439f32082f2f8ed69a1850e0a49c682f0ab38416073c61608a7b391c811170240f18024a5aefe4bb85cb87e16eed4c81b9b0c27a5c4a4db9e3dba1306e872196d46a18813c85bc125c4cd1368e20f8d5e469132ad8515b681b4413cdb24e665480545800552f7c64cf59b972646b647a956e1f235bd30feedb8769b33faec82659474e1c1df46b58e2afa83de50cec02e3eb2a77a7fb17ce35474376b6a4a1fd60d467359dbcf05a04ecf415614b6fda68ac8799dc99b4f2d00c1dafb74864577cae11026c550a6210432a4669b100003000010002a30001080101030803010001acffb409bcc939f831f7a1e5ec88f7a59255ec53040be432027390a4ce896d6f9086f3c5e177fbfe118163aaec7af1462c47945944c4e2c026be5e98bbcded25978272e1e3e079c5094d573f0e83c92f02b32d3513b1550b826929c80dd0f92cac966d17769fd5867b647c3f38029abdc48152eb8f207159ecc5d232c7c1537c79f4b7ac28ff11682f21681bf6d6aba555032bf6f9f036beb2aaa5b3778d6eebfba6bf9ea191be4ab0caea759e2f773a1f9029c73ecb8d5735b9321db085f1b8e2d8038fe2941992548cee0d67dd4547e11dd63af9c9fc1c5466fb684cf009d7197c2cf79e792ab501e6a8a1ca519af2cb9b5f6367e94c0d47502451357be1b50000000000000000000000000000000000000000000000000000";
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