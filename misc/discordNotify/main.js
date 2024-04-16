import { Octokit } from '@octokit/rest';
import fetch from 'node-fetch';

const octokit = new Octokit();
const hookURL = process.env.DISCORD_WEBHOOK;
const maxMessageSize = 1800;

const sendDiscordMessage = async (msg = null) => {
  if (msg !== null && msg.length > maxMessageSize) {
    let cpos = 0;
    let strlen = 0;
    let lastNL = 0;

    while (true) {
      strlen = 0;
      lastNL = -1;

      while ((cpos + strlen) < maxMessageSize) {
        if (msg.charAt(cpos + strlen) === '\n')
          lastNL = strlen;
        if (++strlen === maxMessageSize) {
          strlen = lastNL;
          break;
        }
      }

      if (lastNL !== -1) {
        await sendDiscordMessage(msg.substr(cpos, strlen));
        cpos += strlen + 1;
      }

      if (lastNL === -1 && strlen > 0)
        return sendDiscordMessage(msg.substr(cpos, strlen));
      else if (strlen == 0)
        return null;
    }
  }

  if (hookURL === undefined) {
    console.error('No Discord WebHook URL found!');
    console.log(msg);
    return;
  }

  return fetch(hookURL, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({
      username: 'psOff updates',
      content: msg ?? 'Oopsie'
    })
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
          const msg = `* PR #${pr.number}: ${pr.title}`;
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
