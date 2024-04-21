import { Octokit } from '@octokit/rest';
import fetch from 'node-fetch';

const octokit = new Octokit();
const hookURL = process.env.DISCORD_WEBHOOK;
const maxMessageSize = 2000;

const delay = (ms) => new Promise((resolve) => setTimeout(() => resolve(), ms));

const fetchRetry = ({url, fetchOpts = {}, retryDelay = 5000, retries = 5}) => new Promise((resolve, reject) => {
  const wrap = (n) => {
    fetch(url, fetchOpts)
    .then(async (res) => {
      if (res.ok) {
        const rateRemain = res.headers.get('X-RateLimit-Remaining');
        const rateReset = res.headers.get('X-RateLimit-Reset-After');
        if (rateRemain !== null) {
          if (parseInt(rateRemain) === 1)
            await delay((rateReset ? parseInt(rateReset) : 1) * 1500); // Hold on there, cowboy
        }
        return resolve(res);
      }
      if (n === 0) return reject(`Failed after ${retries} retries.`);
      if (res.status !== 429) return reject(res);
      const jdata = res.json();
      if (typeof jdata.retry_after === 'number') {
        await delay(jdata.retry_after * 1000);
        wrap(--n);
      }
    })
    .catch(async (err) => {
      if (n > 0) {
        await delay(retryDelay);
        wrap(--n);
      } else {
        reject(`Failed after ${retries} retries.`);
      }
    });
  };

  wrap(retries);
});

const sendDiscordMessage = async (msg = null) => {
  if (hookURL === undefined) {
    console.error('No Discord WebHook URL found!');
    console.log(msg);
    return;
  }

  if (msg !== null && msg.length > maxMessageSize) {
    let start = 0;
    let length = 0;
    let lastNL = -1;

    while ((start + length) < msg.length) {
      if (length === maxMessageSize) {
        if (lastNL !== -1) length = lastNL;
        await sendDiscordMessage(msg.substr(start, length));
        start += length + 1;
        length = 0;
      }

      if (msg.charAt(start + length) === '\n') {
        lastNL = length;
      }

      length += 1;
    }

    return sendDiscordMessage(msg.substr(start, length));
  }

  return fetchRetry({
    url: hookURL,
    fetchOpts: {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({
        username: 'psOff updates',
        content: msg ?? 'Oopsie'
      })
    }
  });
};

const r_owner = 'SysRay';
const r_name = 'psOff_public';

const categoryOrder = ['general', 'ench', 'impls', 'bugfixes', 'stubs'];

const guessCategory = (labels) => {
  const cats = [];

  for (const label of labels) {
    switch (label.name) {
      case 'bugfix': cats.push('bugfixes'); break;
      case 'stub': cats.push('stubs'); break;
      case 'implementation': cats.push('impls'); break;
      case 'enhancement': cats.push('ench'); break;
      case 'general': cats.push('general'); break;
      default: break;
    }
  }

  if (cats.length === 0) cats.push('general');
  return cats;
};

const getCategoryName = (cat) => {
  switch (cat) {
    case 'bugfixes': return 'Bugfixes 🪳';
    case 'stubs': return 'Stubbed functions 🆒';
    case 'impls': return 'Implementations 🥳';
    case 'general': return 'General ✅';
    case 'ench': return 'Enhancements 🧙';
  }

  throw new Error('Unknown category name: ' + String.toString(cat));
};

octokit.repos.listReleases({repo: r_name, owner: r_owner, per_page: 2, page: 1}).then(({data}) => {
  const lastRelease = data[0], prevRelease = data[1];

  return new Promise((resolve, reject) => {
    const readPRs = async (pagenum, list = null, retries = 0) => {
      const out = list ?? {general: [], bugfixes: [], stubs: [], impls: [], ench: []};

      const query = [];
      query.push(`repo:${r_owner}/${r_name}`);
      query.push('is:pr is:closed base:features sort:author-date-asc');
      query.push(`merged:${prevRelease.created_at}..${lastRelease.created_at}`);

      return octokit.search.issuesAndPullRequests({q: query.join(' '), per_page: 100, page: pagenum}).then(({data}) => {
        data.items.forEach((pr) => {
          const msg = `* PR #${pr.number}: ${pr.title} ([@${pr.user.login}](<https://github.com/${pr.user.login}/>))`;
          guessCategory(pr.labels).forEach((cat) => out[cat].push(msg));
        });

        if (data.total_count > (out.length - 1)) {
          readPRs(pagenum + 1, out, retries = 0);
        } else {
          const final = [`# Changelog ${prevRelease.tag_name} => ${lastRelease.tag_name}`];
          categoryOrder.forEach((cat) => {
            if (out[cat].length > 0)
              final.push(`\n## ${getCategoryName(cat)}\n${out[cat].join('\n')}`);
          });
          resolve(final.join('\n'));
        }
      }).catch((err) => {
        if (retries == 10) return reject(err);
        try {
          console.error(`HTTP code #${err.status}: ${err.response.data.message}`);
        } catch (e) {
          console.error(err);
        }
        console.error(`Retrying in 1 min (${++retries}/10)...`);
        setTimeout(() => readPRs(pagenum, out), 60000);
      });
    };

    return readPRs(1);
  });
}).then((markdown) => sendDiscordMessage(markdown)).catch((err) => {
  console.error(err);
});
