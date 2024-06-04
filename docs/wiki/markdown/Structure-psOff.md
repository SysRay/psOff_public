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
            <td></td>
        </tr>
        <tr>
            <td>unwinding/</td>
            <td></td>
        </tr>
        <!-- Documentation thingies -->
        <tr>
            <td rowspan=3>docs/</td>
            <td>json/</td>
            <td></td>
        </tr>
        <tr>
            <td>resource/</td>
            <td></td>
        </tr>
        <tr>
            <td>wiki/</td>
            <td></td>
        </tr>
        <!-- Event thingies -->
        <tr>
            <td rowspan=2>eventsystem/</td>
            <td>events/</td>
            <td></td>
        </tr>
        <tr>
            <td>test/</td>
            <td></td>
        </tr>
        <!-- NodeJS thingies -->
        <tr>
            <td rowspan=2>misc/</td>
            <td>discordNotify/</td>
            <td></td>
        </tr>
        <tr>
            <td>testRunner/</td>
            <td></td>
        </tr>
        <!-- User thingies -->
        <tr>
            <td rowspan=2>modules/</td>
            <td>external/</td>
            <td></td>
        </tr>
        <tr>
            <td>internal/</td>
            <td></td>
        </tr>
        <tr>
            <td>modules_include/</td>
            <td>*.h</td>
            <td></td>
        </tr>
        <!-- Testbench thingies -->
        <tr>
            <td>tests/</td>
            <td>*.h</td>
            <td></td>
        </tr>
        <!-- Tools -->
        <tr>
            <td rowspan=5>tools/</td>
            <td>config_emu/</td>
            <td></td>
        </tr>
        <tr>
            <td>dll2nids/</td>
            <td></td>
        </tr>
        <tr>
            <td>gamereports/</td>
            <td></td>
        </tr>
        <tr>
            <td>logging/</td>
            <td></td>
        </tr>
        <tr>
            <td>matchers/</td>
            <td></td>
        </tr>
        <!-- Utilities -->
        <tr>
            <td>utility/</td>
            <td>*.cpp, *.h</td>
            <td></td>
        </tr>
    </tbody>
</table>
