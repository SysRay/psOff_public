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

    const testProc = spawn(binPath, [`--gtest_output=json:${testPath}`]);
    testProc.stdout.pipe(process.stdout);
    testProc.on('exit', (code) => {
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

  fs.open(argv[2], 'w', (err, md_fd) => {
    if (err != null) {
      console.error('Failed to open output markdown file: %s', err);
      process.exit(1);
    }

    fs.writeSync(md_fd, '# Test results🚀\n\n');
    Promise.all(procs).then((values) => {
      values.forEach((ret) => {
        fs.readFile(ret[0], 'utf-8', (err, data) => {
          const jdata = JSON.parse(data);
          fs.writeSync(md_fd, `## ${jdata.name} (${jdata.time})\n`);
          fs.writeSync(md_fd, '| Test class | ✅ Passed | ❌ Failed | ⏭️Skipped | ⏳Time |\n');
          fs.writeSync(md_fd, '|:-----------|:------:|:------:|:-------:|:----:|\n');
          jdata.testsuites.forEach((test, idx) => {
            const passed = test.tests - test.failures - test.disabled;
            fs.writeSync(md_fd, `|${test.name}|${passed}|${test.failures}|${test.disabled}|${test.time}|\n`);
          });
        });
      });
    });
  });
});
