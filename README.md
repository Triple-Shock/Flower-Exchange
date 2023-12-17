![GitHub Workflow Status (with event)](https://img.shields.io/github/actions/workflow/status/Triple-Shock/Flower-Exchange/msbuild.yml?logo=github&label=Build)


# Flower-Exchange Application

- The flower exchange is a system which supports basic trading. 
- An every given order is replied with an Execution Report by the application indicating the status of the order. 
- Orders sometimes could be rejected due to quantity limitations, invalid flower type, etc.
- The system has a `Client-Server Architecture` with the following components.
	- Trader - `Client`
	- Exchanger - `Server`

### To open the source code as a Visual Studio Project

Open a project or solution > Go to `Flower-Exchange` folder > Select `Flower-Exchange.sln` & Open

- After opening the project, `Exchanger` & `Trader` can be built seperately and can be executed as `Server` & `Client`

### To execute the application

- Download `Exchanger.exe`, `Trader.exe` and all the sample order files(`orders-*.csv`) from the latest release.
- Run the `Exchanger.exe` & `Trader.exe`. Make sure two `.exe` files and `.csv` files are in the same folder.
- Provide the filenames of the order files.
- Execution reports will be generated as `.csv` files(`execution_report_orders-*.csv`) in the same folder.

>*Note* : When running the `*.exe` files, if Windows prompts for permission, ensure to allow and then rerun the `.exe` file.


