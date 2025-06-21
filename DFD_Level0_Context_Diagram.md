# DFD Level 0 - Context Diagram

## STM32 SD Card File Management System

```mermaid
graph TB
    %% External Entities
    User[User/PC Terminal]
    SDCard[SD Memory Card]
    USBHost[USB Host Computer]

    %% Main System
    STM32[STM32F429ZITX<br/>File Management System]

    %% Data Flows
    User -->|Commands & Data| STM32
    STM32 -->|Status & File Lists| User
    STM32 -->|Read/Write Operations| SDCard
    SDCard -->|File Data & Status| STM32
    STM32 -->|USB Mass Storage| USBHost
    USBHost -->|File Access Requests| STM32

    %% Styling
    classDef external fill:#e1f5fe,stroke:#01579b,stroke-width:2px
    classDef system fill:#f3e5f5,stroke:#4a148c,stroke-width:3px

    class User,SDCard,USBHost external
    class STM32 system
```

## System Description:

- **STM32F429ZITX**: Main microcontroller handling file operations
- **User/PC Terminal**: Provides commands via UART interface
- **SD Memory Card**: Stores files and directories
- **USB Host Computer**: Accesses files through USB mass storage interface

## Main Data Flows:

1. **User Commands**: File operations (create, delete, list, update)
2. **File Data**: Read/write operations between STM32 and SD card
3. **Status Information**: System responses and file lists
4. **USB Storage**: Mass storage interface for external access
