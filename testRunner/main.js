import fs from 'fs';
import path from 'path';
import process, { argv } from 'process';
import { spawn } from 'child_process';

const testsDir = '_build/_Release/';

const runTest = (binary) => {
  return new Promise((resolve) => {
    const noExt = path.parse(binary).name;
    const binPath = path.join(testsDir, binary);
    const testPath = `testRunner\\summary\\${noExt}.json`;

    spawn(binPath, [`--gtest_output=json:${testPath}`]).on('exit', (code) => {
      resolve([testPath, code]);
    });
  });
};

fs.readdir(testsDir, (err, files) => {
  if (err !== null) {
    console.error('Could not scan the directory: ', err);
    process.exit(1);
  }

  const procs = [];

  files.forEach((file, idx) => {
    if (/(.*_test\.exe$)/i.test(file)) {
      procs.push(runTest(file));
    }
  });

  const md_fd = process.stdout;
  md_fd.write('# Test results🚀\n\n');
  Promise.all(procs).then((values) => {
    values.forEach((ret) => {
      fs.readFile(ret[0], 'utf-8', (err, data) => {
        if (err !== null) {
          console.error('Failed to open %s: %s', ret[0], err);
          return;
        }

        const jdata = JSON.parse(data);
        md_fd.write(`## ${jdata.name} (${jdata.time})\n`);
        md_fd.write('| Test class | ✅ Passed | ❌ Failed | ⏭️Skipped | ⏳Time |\n');
        md_fd.write('|:-----------|:------:|:------:|:-------:|:----:|\n');
        jdata.testsuites.forEach((test, idx) => {
          const passed = test.tests - test.failures - test.disabled;
          md_fd.write(`|${test.name}|${passed}|${test.failures}|${test.disabled}|${test.time}|\n`);
        });
      });
    });
  });
});
