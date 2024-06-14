# PList.exe

## Description

PList est un programme qui permet d'afficher les informations sur les processus en cours d'exécution sur un système Windows. Il utilise les API Windows pour obtenir les informations sur les processus.

## Fonctionnalités

- Afficher les informations sur les processus basées sur un PID
- Afficher les informations sur les processus basées sur le nom
- Afficher l'ensemble des processus
- Afficher les informations sur les threads d'un processus

## Installation

```sh
git clone <repo_url>
cd plist
```

## Compilation

```sh
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
make
```

## Utilisation

Le programme propose les options suivantes :

> Note : Sans arguments passés le programme affichera l'ensemble des processus.

- `-h`: Afficher le message d'aide
- `-d <pid>`: Afficher les informations sur le processus basées sur un PID
- `<nom_processus>`: Afficher les informations sur le processus basées sur le nom

### Exemples

Pour afficher le message d'aide :

```sh
./PList.exe -h
```

Pour afficher les informations sur le processus basées sur le PID :

```sh
./PList.exe -d 1234
```

Pour afficher les informations sur le processus basées sur le nom :

```sh
./PList.exe chrome
```

## Prérequis

- Système d'exploitation : Windows
- Compilateur : [MinGW](https://mingw-w64.org/doku.php)
- CMake : [Site officiel de CMake](https://cmake.org/download/)

Assurez-vous d'avoir installé les exigences mentionnées ci-dessus avant de compiler et d'exécuter le programme.

## Auteurs

- Hokanosekai

## Références

- [Process Status API](https://docs.microsoft.com/en-us/windows/win32/api/psapi/)
- [NtQueryInformation](https://docs.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-ntqueryinformationprocess)
- [CreateToolhelp32Snapshot](https://docs.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-createtoolhelp32snapshot)
- [OpenThread](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openthread)
- [GetThreadTimes](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getthreadtimes)
- [GetProcessMemoryInfo](https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-getprocessmemoryinfo)
