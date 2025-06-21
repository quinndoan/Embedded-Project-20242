# DFD Level 3 - SD Card Communication Detail

## SPI Communication and SD Card Protocol

```mermaid
graph TB
    %% FATFS Interface
    FATFS_Request[FATFS File System<br/>Requests]

    %% SD Card Interface Processes
    SD_Init_Process[4.1.1<br/>SD Card<br/>Initialization]
    SD_Read_Process[4.1.2<br/>SD Card<br/>Read Operation]
    SD_Write_Process[4.1.3<br/>SD Card<br/>Write Operation]
    SD_Status_Process[4.1.4<br/>SD Card<br/>Status Check]

    %% SPI Communication Layer
    SPI_Interface[4.2<br/>SPI Interface<br/>Layer]
    SPI_Transmit[4.2.1<br/>SPI Transmit<br/>Process]
    SPI_Receive[4.2.2<br/>SPI Receive<br/>Process]
    SPI_Select[4.2.3<br/>CS Control<br/>Process]

    %% SD Card Protocol Layer
    SD_Protocol[4.3<br/>SD Card<br/>Protocol Layer]
    CMD_Processor[4.3.1<br/>Command<br/>Processor]
    Response_Handler[4.3.2<br/>Response<br/>Handler]
    Data_Block_Handler[4.3.3<br/>Data Block<br/>Handler]

    %% SD Card Commands
    CMD0[CMD0<br/>GO_IDLE_STATE]
    CMD8[CMD8<br/>SEND_IF_COND]
    CMD17[CMD17<br/>READ_SINGLE_BLOCK]
    CMD24[CMD24<br/>WRITE_BLOCK]
    CMD55[CMD55<br/>APP_CMD]
    CMD41[CMD41<br/>SEND_OP_COND]

    %% Data Stores
    SPI_Buffer[SPI Buffer<br/>Memory]
    SD_Command_Buffer[SD Command<br/>Buffer]
    SD_Response_Buffer[SD Response<br/>Buffer]
    Sector_Buffer[512-byte<br/>Sector Buffer]

    %% Hardware Interface
    SD_Card[SD Memory Card<br/>Hardware]
    SPI_Hardware[SPI1 Hardware<br/>Interface]

    %% Data Flows
    FATFS_Request -->|Initialize Request| SD_Init_Process
    FATFS_Request -->|Read Request| SD_Read_Process
    FATFS_Request -->|Write Request| SD_Write_Process
    FATFS_Request -->|Status Request| SD_Status_Process

    %% Initialization Flow
    SD_Init_Process -->|Power On| SD_Protocol
    SD_Protocol -->|CMD0| CMD_Processor
    CMD_Processor -->|Command| SD_Command_Buffer
    SD_Command_Buffer -->|SPI Data| SPI_Interface
    SPI_Interface -->|Transmit| SPI_Transmit
    SPI_Transmit -->|MOSI| SPI_Hardware
    SPI_Hardware -->|MISO| SPI_Receive
    SPI_Receive -->|Response| SD_Response_Buffer
    SD_Response_Buffer -->|Process| Response_Handler
    Response_Handler -->|Status| SD_Init_Process

    %% Read Operation Flow
    SD_Read_Process -->|CMD17| CMD_Processor
    CMD_Processor -->|Read Command| SD_Command_Buffer
    SD_Command_Buffer -->|SPI Data| SPI_Interface
    SPI_Interface -->|Transmit| SPI_Transmit
    SPI_Transmit -->|MOSI| SPI_Hardware
    SPI_Hardware -->|MISO| SPI_Receive
    SPI_Receive -->|Data Block| Data_Block_Handler
    Data_Block_Handler -->|512 bytes| Sector_Buffer
    Sector_Buffer -->|File Data| SD_Read_Process

    %% Write Operation Flow
    SD_Write_Process -->|Data| Sector_Buffer
    Sector_Buffer -->|512 bytes| Data_Block_Handler
    Data_Block_Handler -->|Write Token| SD_Protocol
    SD_Protocol -->|CMD24| CMD_Processor
    CMD_Processor -->|Write Command| SD_Command_Buffer
    SD_Command_Buffer -->|SPI Data| SPI_Interface
    SPI_Interface -->|Transmit| SPI_Transmit
    SPI_Transmit -->|MOSI| SPI_Hardware
    SPI_Hardware -->|MISO| SPI_Receive
    SPI_Receive -->|Write Status| Response_Handler
    Response_Handler -->|Status| SD_Write_Process

    %% CS Control
    SPI_Select -->|CS Low| SPI_Hardware
    SPI_Select -->|CS High| SPI_Hardware

    %% Hardware Connection
    SPI_Hardware -->|SPI Bus| SD_Card

    %% Styling
    classDef process fill:#f3e5f5,stroke:#4a148c,stroke-width:2px
    classDef datastore fill:#fff3e0,stroke:#e65100,stroke-width:2px
    classDef hardware fill:#e8f5e8,stroke:#2e7d32,stroke-width:2px
    classDef command fill:#fff8e1,stroke:#f57f17,stroke-width:2px

    class SD_Init_Process,SD_Read_Process,SD_Write_Process,SD_Status_Process,SPI_Interface,SPI_Transmit,SPI_Receive,SPI_Select,SD_Protocol,CMD_Processor,Response_Handler,Data_Block_Handler process
    class SPI_Buffer,SD_Command_Buffer,SD_Response_Buffer,Sector_Buffer datastore
    class SD_Card,SPI_Hardware hardware
    class CMD0,CMD8,CMD17,CMD24,CMD55,CMD41 command
```

## Detailed SD Card Communication Process:

### 4.1 SD Card Operations

#### 4.1.1 SD Card Initialization

- **Purpose**: Initialize SD card and establish communication
- **Sequence**:
  1. Power on sequence (10 dummy bytes)
  2. Send CMD0 (GO_IDLE_STATE)
  3. Send CMD8 (SEND_IF_COND) - Check SD version
  4. Send CMD55 + CMD41 (ACMD41) - Initialize card
  5. Read OCR register (CMD58) - Check capacity

#### 4.1.2 SD Card Read Operation

- **Purpose**: Read 512-byte sector from SD card
- **Sequence**:
  1. Send CMD17 (READ_SINGLE_BLOCK)
  2. Wait for data token (0xFE)
  3. Read 512 bytes of data
  4. Read 2-byte CRC
  5. Deselect card

#### 4.1.3 SD Card Write Operation

- **Purpose**: Write 512-byte sector to SD card
- **Sequence**:
  1. Wait for card ready (0xFF)
  2. Send CMD24 (WRITE_BLOCK)
  3. Send data token (0xFE)
  4. Send 512 bytes of data
  5. Send 2-byte CRC
  6. Wait for write response
  7. Wait for write completion

#### 4.1.4 SD Card Status Check

- **Purpose**: Check SD card status and errors
- **Functions**: `SD_disk_status()`

### 4.2 SPI Interface Layer

#### 4.2.1 SPI Transmit Process

- **Function**: `SPI_TxByte()`, `SPI_TxBuffer()`
- **Purpose**: Transmit data to SD card via SPI
- **Hardware**: SPI1 (MOSI, SCK, CS)

#### 4.2.2 SPI Receive Process

- **Function**: `SPI_RxByte()`, `SPI_RxBytePtr()`
- **Purpose**: Receive data from SD card via SPI
- **Hardware**: SPI1 (MISO, SCK, CS)

#### 4.2.3 CS Control Process

- **Function**: `SELECT()`, `DESELECT()`
- **Purpose**: Control chip select signal
- **Hardware**: PC4 (Chip Select)

### 4.3 SD Card Protocol Layer

#### 4.3.1 Command Processor

- **Function**: `SD_SendCmd()`
- **Purpose**: Format and send SD commands
- **Command Format**: 6 bytes (CMD + ARG + CRC)

#### 4.3.2 Response Handler

- **Purpose**: Process SD card responses
- **Response Types**: R1, R3, R7 (status responses)

#### 4.3.3 Data Block Handler

- **Functions**: `SD_RxDataBlock()`, `SD_TxDataBlock()`
- **Purpose**: Handle 512-byte data blocks
- **Tokens**: 0xFE (data), 0xFC (multiple write), 0xFD (stop)

## Key SD Commands:

### CMD0 - GO_IDLE_STATE

- **Purpose**: Reset SD card to idle state
- **Argument**: 0x00000000
- **CRC**: 0x95

### CMD8 - SEND_IF_COND

- **Purpose**: Check SD card interface condition
- **Argument**: 0x000001AA
- **CRC**: 0x87

### CMD17 - READ_SINGLE_BLOCK

- **Purpose**: Read single 512-byte block
- **Argument**: Sector address

### CMD24 - WRITE_BLOCK

- **Purpose**: Write single 512-byte block
- **Argument**: Sector address

### CMD55 - APP_CMD

- **Purpose**: Application-specific command prefix
- **Argument**: 0x00000000

### CMD41 - SEND_OP_COND

- **Purpose**: Initialize SD card
- **Argument**: HCS bit (0x40000000 for SDHC)

## Hardware Configuration:

- **SPI1**: Master mode, 8-bit data, MSB first
- **Baud Rate**: SPI_BAUDRATEPRESCALER_32
- **Pins**:
  - PA5: SCK (SPI1_SCK)
  - PA6: MISO (SPI1_MISO)
  - PA7: MOSI (SPI1_MOSI)
  - PC4: CS (Chip Select)
