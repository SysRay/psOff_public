# Brief overview of the project structure

<table>
    <thead>
        <tr>
            <th>Path</th>
            <th>Contents</th>
            <th>Description</th>
        </tr>
    </thead>
    <tbody>
        <!-- Workflow thingies -->
        <tr>
            <td rowspan=3>.github/workflows/</td>
            <td>build.yml</td>
            <td>Main building workflow</td>
        </tr>
        <tr>
            <td>release_notify.yml</td>
            <td>Discord release announcements</td>
        </tr>
        <tr>
            <td>upwiki.yml</td>
            <td>Updates the wiki contents</td>
        </tr>
        <!-- VSCode thingies -->
        <tr>
            <td>.vscode/</td>
            <td>*.json</td>
            <td>VSCode configuration files</td>
        </tr>
        <!-- Core thingies -->
        <tr>
            <td rowspan=9>core/</td>
            <td>dmem/</td>
            <td>Direct and Flexible memory manager implementation</td>
        </tr>
        <tr>
            <td>fileManager/</td>
            <td>All the IO things (devices, files, file representation of elf library, etc.)</td>
        </tr>
        <tr>
            <td>initParams/</td>
            <td>Arguments parser, kicks in when you pass some arguments to psoff.exe</td>
        </tr>
        <tr>
            <td>kernel/</td>
            <td>All the kernel things (eventqueue, filesystem, multithreading stuff)</td>
        </tr>
        <tr>
            <td>memory/</td>
            <td>RAM/GPU memory allocation stuff</td>
        </tr>
        <tr>
            <td>runtime/</td>
            <td>ELF64 parser, runtime linker, interception mechanisms, exception handler and other funny stuff</td>
        </tr>
        <tr>
            <td>systemContent/</td>
            <td>Parser for param.sfo files</td>
        </tr>
        <tr>
            <td>timer/</td>
            <td>All the clock stuff</td>
        </tr>
        <tr>
            <td>unwinding/</td>
            <td>Thread unwinding</td>
        </tr>
        <!-- Documentation thingies -->
        <tr>
            <td rowspan=3>docs/</td>
            <td>json/</td>
            <td>Json schemas for emulator config files</td>
        </tr>
        <tr>
            <td>resource/</td>
            <td>Useful PS4 hardware related documentation</td>
        </tr>
        <tr>
            <td>wiki/</td>
            <td>This folder is used by upwiki.yml workflow for wiki updating</td>
        </tr>
        <!-- Event thingies -->
        <tr>
            <td rowspan=2>eventsystem/</td>
            <td>events/</td>
            <td>Our event system</td>
        </tr>
        <tr>
            <td>test/</td>
            <td>Google unit tests</td>
        </tr>
        <!-- NodeJS thingies -->
        <tr>
            <td rowspan=2>misc/</td>
            <td>discordNotify/</td>
            <td>Script for discord release notifications, used by release_notify.yml workflow</td>
        </tr>
        <tr>
            <td>testRunner/</td>
            <td>Script for Google tests running, used by build.yml</td>
        </tr>
        <!-- User thingies -->
        <tr>
            <td rowspan=2>modules/</td>
            <td>external/</td>
            <td>Libraries with PS4 user functions, all the sce* exports goes there</td>
        </tr>
        <tr>
            <td>internal/</td>
            <td>Set of internal things for calling from exported sce functions</td>
        </tr>
        <tr>
            <td>modules_include/</td>
            <td>*.h</td>
            <td>Common includes for all the sce modules</td>
        </tr>
        <!-- Testbench thingies -->
        <tr>
            <td>tests/</td>
            <td>*.h</td>
            <td>Google unit tests</td>
        </tr>
        <!-- Tools -->
        <tr>
            <td rowspan=5>tools/</td>
            <td>config_emu/</td>
            <td>Emulator's configuration file parser</td>
        </tr>
        <tr>
            <td>dll2nids/</td>
            <td>This program patches compiled sce libraries, by replacing function names with nids</td>
        </tr>
        <tr>
            <td>gamereports/</td>
            <td></td>
        </tr>
        <tr>
            <td>logging/</td>
            <td>Baical p7 logger integration</td>
        </tr>
        <tr>
            <td>matchers/</td>
            <td>Error matchers for GitHub build.yml workflow</td>
        </tr>
        <!-- Utilities -->
        <tr>
            <td>utility/</td>
            <td>*.cpp, *.h</td>
            <td>Set of handy functions</td>
        </tr>
    </tbody>
</table>
