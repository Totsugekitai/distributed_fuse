# distributed-fuse

read-only NFS like file system

## dependency

- `mochi-margo@0.8.2`
- `mercury`

## build

```sh
$ make
```

## usage

```sh
$ spack load mochi-margo@0.8.2
$ ./dfs_server
Server running at address <This is Server address>
```

```sh
$ ./dfs_client --server=<Server address> /path/to/mountpoint
```

```sh
$ ls /path/to/mountpoint
$ stat /path/to/mountpoint
$ cat /path/to/mountpoint/file
$ etc...(read only)
```
