# DFD Level 1 - Detailed System Decomposition

## STM32 SD Card File Management System

```mermaid
graph TB
    %% External Entities
    User[User/PC Terminal]
    SDCard[SD Memory Card]
    USBHost[USB Host Computer]

    %% Main Processes
    UART_Process[1.0<br/>UART Communication<br/>Process]
    Menu_Process[2.0<br/>Menu System<br/>Process]
    File_Ops[3.0<br/>File Operations<br/>Process]
    SD_Interface[4.0<br/>SD Card Interface<br/>Process]
    FATFS_Process[5.0<br/>FATFS File System<br/>Process]
    USB_Process[6.0<br/>USB Mass Storage<br/>Process]

    %% Data Stores
    File_Buffer[File Buffer<br/>Memory]
    Command_Buffer[Command Buffer<br/>Memory]
    SD_Buffer[SD Card Buffer<br/>Memory]

    %% Data Flows
    User -->|UART Commands| UART_Process
    UART_Process -->|Parsed Commands| Command_Buffer
    Command_Buffer -->|User Input| Menu_Process
    Menu_Process -->|Operation Requests| File_Ops
    File_Ops -->|File Operations| FATFS_Process
    FATFS_Process -->|SD Commands| SD_Interface
    SD_Interface -->|SPI Data| SDCard
    SDCard -->|File Data| SD_Buffer
    SD_Buffer -->|File Content| FATFS_Process
    FATFS_Process -->|File Results| File_Ops
    File_Ops -->|Status Messages| UART_Process
    UART_Process -->|System Responses| User

    %% USB Path
    USBHost -->|USB Requests| USB_Process
    USB_Process -->|File Access| FATFS_Process
    FATFS_Process -->|USB Data| USB_Process
    USB_Process -->|File Content| USBHost

    %% Internal Data Flows
    File_Ops -->|File Data| File_Buffer
    File_Buffer -->|Content| UART_Process

    %% Styling
    classDef external fill:#e1f5fe,stroke:#01579b,stroke-width:2px
    classDef process fill:#f3e5f5,stroke:#4a148c,stroke-width:2px
    classDef datastore fill:#fff3e0,stroke:#e65100,stroke-width:2px

    class User,SDCard,USBHost external
    class UART_Process,Menu_Process,File_Ops,SD_Interface,FATFS_Process,USB_Process process
    class File_Buffer,Command_Buffer,SD_Buffer datastore
```

## Process Descriptions:

### 1.0 UART Communication Process

- **Input**: User commands via UART
- **Output**: Parsed commands to command buffer
- **Functions**:
  - `uart_receive_string()` - Receive user input
  - `uart_receive_char()` - Receive single character
  - `send_uart()` - Send responses to user

### 2.0 Menu System Process

- **Input**: Parsed commands from command buffer
- **Output**: Operation requests to file operations
- **Functions**:
  - Menu display and option selection
  - Command validation and routing

### 3.0 File Operations Process

- **Input**: Operation requests from menu system
- **Output**: File operation requests to FATFS
- **Functions**:
  - File creation, deletion, listing
  - Directory creation and deletion
  - File content updates

### 4.0 SD Card Interface Process

- **Input**: SD commands from FATFS
- **Output**: SPI data to SD card
- **Functions**:
  - `SD_disk_initialize()` - Initialize SD card
  - `SD_disk_read()` - Read sectors
  - `SD_disk_write()` - Write sectors
  - SPI communication protocols

### 5.0 FATFS File System Process

- **Input**: File operations from file operations process
- **Output**: SD commands to SD interface
- **Functions**:
  - `f_mount()` - Mount file system
  - `f_open()`, `f_close()` - File handling
  - `f_read()`, `f_write()` - File I/O
  - `f_mkdir()`, `f_unlink()` - Directory operations

### 6.0 USB Mass Storage Process

- **Input**: USB requests from host computer
- **Output**: File access to FATFS
- **Functions**:
  - USB MSC (Mass Storage Class) interface
  - File system emulation for USB host

## Data Stores:

- **File Buffer**: Temporary storage for file content
- **Command Buffer**: Storage for parsed user commands
- **SD Buffer**: Buffer for SD card data transfer
