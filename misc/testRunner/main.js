import fs from 'fs/promises';
import path from 'path';
import process, { argv } from 'process';
import { spawn } from 'child_process';

const testsDir = '_build/_Release/';

const runTest = (binary) => {
  return new Promise((resolve) => {
    const noExt = path.parse(binary).name;
    const binPath = path.join(testsDir, binary);
    const testPath = `testRunner\\summary\\${noExt}.json`;
    let testTimer = null;

    const proc = spawn(binPath, [`--gtest_output=json:${testPath}`]).on('exit', (code) => {
      if (testTimer != null) clearTimeout(testTimer);
      resolve([testPath, binPath, code]);
    });

    testTimer = setTimeout(() => {
      proc.kill();
      testTimer = null;
    }, 2000);
  });
};

fs.readdir(testsDir).then((files) => {
  const procs = [];

  files.forEach((file, idx) => {
    if (/(.*_test\.exe$)/i.test(file)) {
      procs.push(runTest(file));
    }
  });

  const md_fd = process.stdout;
  md_fd.write('# Test results🚀\n\n');
  Promise.all(procs).then((values) => {
    const iops = [];
    let someCrashed = false;

    values.forEach((ret) => {
      if (ret[2] !== 0) {
        someCrashed = true;
        return;
      }

      iops.push(fs.readFile(ret[0], 'utf-8').then((data) => {
        try {
          const jdata = JSON.parse(data);
          md_fd.write(`## ${jdata.name} (${jdata.time})\n`);
          md_fd.write('| Test class | ✅ Passed | ❌ Failed | ⏭️Skipped | ⏳Time |\n');
          md_fd.write('|:-----------|:------:|:------:|:-------:|:----:|\n');
          jdata.testsuites.forEach((test, idx) => {
            const passed = test.tests - test.failures - test.disabled;
            md_fd.write(`|${test.name}|${passed}|${test.failures}|${test.disabled}|${test.time}|\n`);
          });
        } catch (ex) {
          console.error(ex);
        }
      }).catch((err) => {
        console.error('Failed to open %s: %s', ret[0], err);
      }));
    });

    Promise.all(iops).then(() => {
      if (someCrashed === true) {
        md_fd.write('\n## Some tests were crashed or killed!😱\n');
        values.forEach((ret) => ret[2] !== 0 ? md_fd.write(`* ${ret[1]}: ${ret[2] == null ? 'timeout' : ret[2]}\n`) : false);
        md_fd.write('\n');
      }
    });
  });
}).catch((err) => {
  console.error('Could not scan the directory: ', err);
  process.exit(1);
});
