# BAT-LZ
Implementation of variants of the Bounded Access Time Lempel-Ziv (BAT-LZ) algorithm


## Installation

```bash
git clone https://github.com/fmasillo/BAT-LZ
cd BAT-LZ
make
```

This will create the following executables:
- `baseline1_BATLZ`
- `baseline2_BATLZ`
- `greedy_BATLZ`
- `minmax_BATLZ`
- `greedier_BATLZ`
- `uncompress`

`uncompress` is a simple program to uncompress the files compressed by the previous programs.

To compute the Suffix Array, this implementation uses a script called gensa in kkp [1].
To generate the Suffix Tree, this implementation uses a modified code of Dotan Tsadok.

## References

[1]: Juha Karkkainen, Dominik Kempa and Simon J. Puglisi, "Linear Time Lempel-Ziv Factorization: Simple, Fast, Small." In Proc. CPM 2013, LNCS vol. 7922, pp. 189-200. Springer 2013.


## Usage

```bash
./<variant> <input_file> <maximum_chain_length>
```

Where `<variant>` is one of the following:
- `baseline1_BATLZ`
- `baseline2_BATLZ`
- `greedy_BATLZ`
- `minmax_BATLZ`
- `greedier_BATLZ`

`<input_file>` is the file to be compressed, and `<maximum_chain_length>` is the maximum chain length to be used in the compression.

## Example

```bash
./baseline1_BATLZ test_file.txt 2
```

Every variant will print in stdout the compressed file, and the number of BAT-LZ phrases. In stderr, it will print some statistics about the compression process.

To check the correctness of the compression, you can use the following command:

```bash
./uncompress <compressed_file>
```

Where `<compressed_file>` is the file obtained from the compression. Again, this will print in stdout the uncompressed file.


## Acknowledgements

### Theoretical Results
- Zsuzsanna Lipták
- Francesco Masillo
- Gonzalo Navarro

### Implementation
- Francesco Masillo
- Gonzalo Navarro

## Citation

Zsuzsanna Lipták, Francesco Masillo, Gonzalo Navarro. BAT-LZ Out of Hell, arXiv (https://arxiv.org/html/2403.09893v1)

bibtex:
```
@article{liptak2024bat,
  title={BAT-LZ out of hell},
  author={Lipt{\'a}k, {\relax Zs}uzsanna and Masillo, Francesco and Navarro, Gonzalo},
  journal={arXiv preprint arXiv:2403.09893},
  year={2024}
}
```

