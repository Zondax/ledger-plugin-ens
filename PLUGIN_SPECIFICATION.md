# Technical Specification
## About

This documentation describes the smart contracts and functions supported by the ENS plugin.

## Smart Contracts

Smart contracts covered by this plugin are:

|  Network | Smart Contract                 | Smart Contract Address |
| -------- | ------------------------------ | ---------------------- |
| Ethereum | ETH Registrar Controller       | [0x253553366da8546fc250f225fe3d25d0c782303b](https://etherscan.io/address/0x253553366da8546fc250f225fe3d25d0c782303b) |
| Ethereum | ETH Registrar Controller Old   | [0x283af0b28c62c092c9727f1ee09c02ca627eb7f5](https://etherscan.io/address/0x283af0b28c62c092c9727f1ee09c02ca627eb7f5) |
| Ethereum | Reverse Registrar              | [0xa58E81fe9b61B5c3fE2AFD33CF304c454AbFc7Cb](https://etherscan.io/address/0xa58E81fe9b61B5c3fE2AFD33CF304c454AbFc7Cb) |
| Ethereum | Reverse Registrar Old          | [0x9062C0A6Dbd6108336BcBe4593a3D1cE05512069](https://etherscan.io/address/0x9062C0A6Dbd6108336BcBe4593a3D1cE05512069) |
| Ethereum | Reverse Registrar Old 2        | [0x084b1c3C81545d370f3634392De611CaaBFf8148](https://etherscan.io/address/0x084b1c3C81545d370f3634392De611CaaBFf8148) |
| Ethereum | Bulk Renewal                   | [0xff252725f6122a92551a5fa9a6b6bf10eb0be035](https://etherscan.io/address/0xff252725f6122a92551a5fa9a6b6bf10eb0be035) |
| Ethereum | StaticBulkRenewal              | [0xa12159e5131b1eEf6B4857EEE3e1954744b5033A](https://etherscan.io/address/0xa12159e5131b1eEf6B4857EEE3e1954744b5033A) |
| Ethereum | ENS Registry With Fallback     | [0x00000000000c2e074ec69a0dfb2997ba6c7d2e1e](https://etherscan.io/address/0x00000000000c2e074ec69a0dfb2997ba6c7d2e1e) |
| Ethereum | DNS Registrar                  | [0x58774bb8acd458a640af0b88238369a167546ef2](https://etherscan.io/address/0x58774bb8acd458a640af0b88238369a167546ef2) |
| Ethereum | Public Resolver                | [0x231b0Ee14048e9dCcD1d247744d114a4EB5E8E63](https://etherscan.io/address/0x231b0Ee14048e9dCcD1d247744d114a4EB5E8E63) |

## Functions implemented:


|    Function               | Selector | Displayed Parameters   | 
| ---                       | ---      | ---                    |
|commit                     |0xf14fcbc8|<code>bytes32 commitment</code> |
|register                   |0x85f6d155| <table> <tbody> <tr><td><code>string name</code></td></tr> <tr><td><code>address owner</code></td></tr> <tr><td><code>uint256 duration</code></td></tr> <tr><td><code>bytes32 secret</code></td></tr> </tbody> </table>                               |
|registerWithConfig         |0xf7a16963|<table> <tbody> <tr><td><code>string name</code></td></tr> <tr><td><code>address owner</code></td></tr> <tr><td><code>bytes32 secret</code></td></tr>  <tr><td><code>address resolver</code></td></tr> <tr><td><code>address addr</code></td></tr></tbody> </table>|
|renew                      |0xacf1a841|<table> <tbody> <tr><td><code>string name</code></td></tr> <tr><td><code>uint256 duration</code></td></tr> </tbody> </table>|
|setName                    |0xc47f0027|<code>string name</code>|
|renewAll*                   |0xe8d6dbb4|<table> <tbody> <tr><td><code>string names</code></td></tr> <tr><td><code>uint256 duration</code></td></tr> <tr><td><code>bytes proof</code></td></tr> </tbody> </table> |
|proveAndClaim*              |0x8bbedf75|<table> <tbody> <tr><td><code>bytes names</code></td></tr>  <tr><td><code>uint256 #inputs</code></td></tr><tr><td><code>bytes proof</code></td></tr></tbody> </table>|
|proveAndClaimWithResolver*  |0x224199c2|<table> <tbody> <tr><td><code>bytes names</code></td></tr>  <tr><td><code>uint256 #inputs</code></td></tr><tr><td><code>bytes proof</code></td></tr><tr><td><code>address resolver</code></td></tr> <tr><td><code>address addr</code></td></tr> </tbody> </table>|
|setOwner                   |0x5b0fc9c3|<table> <tbody> <tr><td><code>bytes32 node</code></td></tr> <tr><td><code>address owner</code></td></tr> </tbody> </table>|
|setResolver                |0x1896f70a|<table> <tbody> <tr><td><code>bytes32 node</code></td></tr> <tr><td><code>address resolver</code></td></tr> </tbody> </table>||
|SetSubnodeRecord           |0x5ef2c7f0|<table> <tbody> <tr><td><code>bytes32 node</code></td></tr>  <tbody> <tr><td><code>bytes32 label</code></td></tr><tr><td><code>address owner</code></td></tr> <tr><td><code>address resolver</code></td></tr> <tr><td><code>uint256 ttl</code></td></tr></tbody> </table>||
|SetAddr*           |0x8b95dd71|<table> <tbody> <tr><td><code>bytes32 node</code></td></tr><tr><td><code>uint256 coinType</code></tbody> <tr><td><code>bytes a</td></tr></code></td</table>||
|SetText*           |0x10f13a8c|<table> <tbody> <tr><td><code>bytes32 node</code></td></tr><tr><td><code>string Key</code><tr><td><code>string Value</code></tbody> </table>||
|SetContentHash*           |0x304e6ade|<table> <tbody> <tr><td><code>bytes32 node</code></td></tr><tr><td><code>bytes hash</code></td></code></tbody> </table>||
|Multicall*           |0xac9650d8|<table> <tbody> <tr><td><code>bytes calls</code></td></tr></tbody> </table>|


*For renewAll, ENS plugin will only support signing transactions with 2 names maximum.
For multicall, ENS plugin will only support signing transactions with 3 calls maximum.
For strings and byte arrays bigger then 32, plugin is showing the first and last 16 bytes in "16...16" format, due to memory limitations.
