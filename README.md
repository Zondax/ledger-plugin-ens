[![Code style check](https://github.com/zondax/ledger-plugin-ens/actions/workflows/lint-workflow.yml/badge.svg)](https://github.com/zondax/ledger-plugin-ens/actions/workflows/lint-workflow.yml)
[![Compilation & tests](https://github.com/zondax/ledger-plugin-ens/actions/workflows/ci-workflow.yml/badge.svg)](https://github.com/zondax/ledger-plugin-ens/actions/workflows/ci-workflow.yml)

# Ledger ENS Plugin

This is a plugin for the Ethereum application which helps parsing and displaying relevant information when signing a ENS smart contract.

## Prerequisite

Clone the plugin to a new folder.

```shell
git clone https://github.com/Zondax/app-plugin-ens.git
```

Then in the same folder clone two more repositories, which is the plugin-tools and app-ethereum.

```shell
git clone https://github.com/LedgerHQ/plugin-tools.git                          #plugin-tools
git clone --recurse-submodules https://github.com/LedgerHQ/app-ethereum.git     #app-ethereum
```
## Documentation

Need more information about the interface, the architecture, or general stuff about ethereum plugins? You can find more about them in the [ethereum-app documentation](https://github.com/LedgerHQ/app-ethereum/blob/master/doc/ethapp_plugins.adoc).

## Smart Contracts

Smart contracts covered by this plugin are:

|  Network | Smart Contract                 | Smart Contract Address |
| -------- | ------------------------------ | ---------------------- |
| Ethereum | ETH Registrar Controller       | [0x253553366da8546fc250f225fe3d25d0c782303b](https://etherscan.io/address/0x253553366da8546fc250f225fe3d25d0c782303b) |
| Ethereum| ETH Registrar Controller Old    | [0x283af0b28c62c092c9727f1ee09c02ca627eb7f5](https://etherscan.io/address/0x283af0b28c62c092c9727f1ee09c02ca627eb7f5) |
| Ethereum| Reverse Registrar               | [0xa58E81fe9b61B5c3fE2AFD33CF304c454AbFc7Cb](https://etherscan.io/address/0xa58E81fe9b61B5c3fE2AFD33CF304c454AbFc7Cb) |
| Ethereum| Reverse Registrar Old           | [0x9062C0A6Dbd6108336BcBe4593a3D1cE05512069](https://etherscan.io/address/0x9062C0A6Dbd6108336BcBe4593a3D1cE05512069) |
| Ethereum| Reverse Registrar Old 2         |[0x084b1c3C81545d370f3634392De611CaaBFf8148](https://etherscan.io/address/0x084b1c3C81545d370f3634392De611CaaBFf8148) |
| Ethereum| Bulk Renewal                    | [0xff252725f6122a92551a5fa9a6b6bf10eb0be035](https://etherscan.io/address/0xff252725f6122a92551a5fa9a6b6bf10eb0be035) |
| Ethereum| StaticBulkRenewal               | [0xa12159e5131b1eEf6B4857EEE3e1954744b5033A](https://etherscan.io/address/0xa12159e5131b1eEf6B4857EEE3e1954744b5033A) |
| Ethereum| ENS Registry With Fallback      | [0x00000000000c2e074ec69a0dfb2997ba6c7d2e1e](https://etherscan.io/address/0x00000000000c2e074ec69a0dfb2997ba6c7d2e1e) |
| Ethereum| DNS Registrar                   | [0x58774bb8acd458a640af0b88238369a167546ef2](https://etherscan.io/address/0x58774bb8acd458a640af0b88238369a167546ef2) |


## Functions implemented:


|    Function               | Selector | Displayed Parameters   | 
| ---                       | ---      | ---                    |
|commit                     |0xf14fcbc8|<code>bytes32 commitment</code> |
|register                   |0x85f6d155| <table> <tbody> <tr><td><code>string name</code></td></tr> <tr><td><code>address owner</code></td></tr> <tr><td><code>uint256 duration</code></td></tr> <tr><td><code>bytes32 secret</code></td></tr> </tbody> </table>                               |
|registerWithConfig         |0xf7a16963|<table> <tbody> <tr><td><code>string name</code></td></tr> <tr><td><code>address owner</code></td></tr> <tr><td><code>bytes32 secret</code></td></tr>  <tr><td><code>address resolver</code></td></tr> <tr><td><code>address addr</code></td></tr></tbody> </table>|
|renew                      |0xacf1a841|<table> <tbody> <tr><td><code>string name</code></td></tr> <tr><td><code>uint256 duration</code></td></tr> </tbody> </table>|
|setName                    |0xc47f0027|<code>string name</code>|
|renewAll                   |0xe8d6dbb4|<table> <tbody> <tr><td><code>uint256 #names</code></td></tr> <tr><td><code>address owner</code></td></tr> <tr><td><code>uint256 duration</code></td></tr> </tbody> </table> |
|proveAndClaim              |0x8bbedf75|<code>uint256 #inputs</code>|
|proveAndClaimWithResolver  |0x224199c2|<table> <tbody> <tr><td><code>address resolver</code></td></tr> <tr><td><code>address addr</code></td></tr> </tbody> </table>|
|setOwner                   |0x5b0fc9c3|<table> <tbody> <tr><td><code>bytes32 node</code></td></tr> <tr><td><code>address owner</code></td></tr> </tbody> </table>|
|setResolver                |0x1896f70a|<table> <tbody> <tr><td><code>bytes32 node</code></td></tr> <tr><td><code>address resolver</code></td></tr> </tbody> </table>||
|SetSubnodeRecord           |0x5ef2c7f0|<table> <tbody> <tr><td><code>bytes32 node</code></td></tr>  <tbody> <tr><td><code>bytes32 label</code></td></tr><tr><td><code>address owner</code></td></tr> <tr><td><code>address resolver</code></td></tr> <tr><td><code>uint256 ttl</code></td></tr></tbody> </table>||


## Build

Go to the plugin-tools folder and run the "./start" script.
```shell
cd plugin-tools  # go to plugin folder
./start.sh       # run the script start.sh
```
The script will build a docker image and attach a console.
When the docker image is running go to the "app-plugin-ens" folder and build the ".elf" files.
```shell
cd app-plugin-ens/tests       # go to the tests folder in app-plugin-ens
./build_local_test_elfs.sh      # run the script build_local_test_elfs.sh
```

## Tests

To test the plugin go to the tests folder from the "app-plugin-ens" and run the script "test"
```shell
cd app-plugin-ens/tests         # go to the tests folder in app-plugin-ens
yarn test                       # run the script test
```
