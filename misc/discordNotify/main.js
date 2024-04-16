import { Octokit } from '@octokit/rest';
import fetch from 'node-fetch';

const octokit = new Octokit();
const hookURL = process.env.DISCORD_WEBHOOK;

const sendDiscordMessage = (msg = null) => {
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
  for (const label of labels) {
    switch (label.name) {
      case 'bugfix': return 'bugfixes';
      case 'stub': return 'stubs';
      case 'implementation': return 'impls';
      case 'enhancement': return 'ench';
      default: break;
    }
  }

  return 'general';
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
          out[guessCategory(pr.labels)].push(`* PR #${pr.number}: ${pr.title}`);
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
  console.err(err);
});
