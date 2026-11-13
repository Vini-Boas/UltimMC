<p align="center">

<img width="150" src="https://github.com/UltimMC/Launcher/assets/62727185/48d60c15-23ae-4d48-84b8-813f72db6cdd">

<p align="center">UltimMC is a custom launcher for Minecraft which allows you to manage multiple instances and use offline ("cracked") accounts while keeping as close as possible to the original.</p>

</p>

> [!IMPORTANT]
> This project is a **fork** of MultiMC. </br> </br>
> This software is provided without any warranty, so please don't contact the main
> MultiMC developers in case anything goes wrong using this launcher. </br> </br>
> Nonetheless, feel free to create an issue within this repository
> in case you face an issue specific to UltimMC.

## Downloading

Download the [latest release](https://github.com/Vini-Boas/UltimMC/releases/latest) for your system:

- *[Windows \(32-bit, also runs on 64-bit Windows\)](https://github.com/Vini-Boas/UltimMC/releases/latest/download/mmc-cracked-win32.zip)*.

- *[Linux (64-bit)](https://github.com/Vini-Boas/UltimMC/releases/latest/download/mmc-cracked-lin64.zip)*.

- *[macOS (10.14 and newer)](https://github.com/Vini-Boas/UltimMC/releases/latest/download/mmc-cracked-osx64.zip)*.

> [!NOTE]
> In the case you're using macOS then another additional step you might need to do
> is to make `UltimMC` an executable by running the command `chmod +x UltimMC.app/Contents/MacOS/UltimMC` in the terminal.

### Nightly builds

If you want the newest changes before they are released, there is a [nightly build](https://github.com/Vini-Boas/UltimMC/releases/tag/nightly). It is refreshed on every change to `develop`, is not a stable release, and may be broken.

- *[Windows](https://github.com/Vini-Boas/UltimMC/releases/download/nightly/mmc-cracked-win32.zip)*, *[Linux](https://github.com/Vini-Boas/UltimMC/releases/download/nightly/mmc-cracked-lin64.zip)* and *[macOS](https://github.com/Vini-Boas/UltimMC/releases/download/nightly/mmc-cracked-osx64.zip)*.

## Installing and Using

1. Pick the correct download for your system.
2. Uncompress it in your desired directory.
3. Launch `UltimMC`.
4. Go to account settings.
6. A. Pick "Add Local" and you will be requested to use the username you desire, this can be anything.
7. B. Pick "Add Ely.by" and add your Ely.by account by putting your username and password.
8. Save it.
9. Now enjoy the Launcher.

## Updating

To update the launcher replace all replaceable files and folders with the newer ones from any of the links listed above.

A better update system is in the works.

## Microsoft accounts

This build does **not** ship a Microsoft client ID, so Microsoft login is off until you provide your own.
Local and Ely.by accounts work without it.

1. Register an application in [Microsoft Entra](https://learn.microsoft.com/en-us/entra/identity-platform/quickstart-register-app) for personal accounts, with no redirect URI and "public client flows" enabled.
2. Give the launcher its client ID, in either of these ways:
   - set `MSAClientID=<your client ID>` in the launcher's config file (`ultimmc.cfg`), or
   - set the `ULTIMMC_MSA_CLIENT_ID` environment variable (it takes priority over the config file).
3. Restart the launcher.

Using a client ID means you accept the [Microsoft Identity Platform Terms of Use](https://learn.microsoft.com/en-us/legal/microsoft-identity-platform/terms-of-use).
Mojang may also require your application to be approved before Minecraft login works; this is outside the launcher's control.

Accounts you already have stay saved while no ID is set. They are simply not refreshed or launched until one is provided.

People distributing their own builds can bake an ID in with `-DLauncher_MSA_CLIENT_ID=<id>`.
