<%*
// Log one interview round against an application tracker.
const ACTIVE = "16-Interview-Command-Center/03-Pipeline/Active/";
const apps = app.vault.getMarkdownFiles()
  .filter(f => f.path.startsWith(ACTIVE) && app.metadataCache.getFileCache(f)?.frontmatter?.stage)
  .sort((a, b) => a.basename.localeCompare(b.basename));
const appFile = await tp.system.suggester(apps.map(f => f.basename), apps, false, "Which application?");
const round = (await tp.system.prompt("Round (for example: Phone screen, Onsite 2 system design)")) ?? "";
const date = (await tp.system.prompt("Date (YYYY-MM-DD)", tp.date.now("YYYY-MM-DD"))) ?? tp.date.now("YYYY-MM-DD");
const stages = ["recruiter", "OA", "phone", "onsite", "offer", "rejected", "withdrawn"];
const newStage = await tp.system.suggester(["Leave stage unchanged", ...stages], [null, ...stages], false, "Stage after this round");
const fm = appFile ? (app.metadataCache.getFileCache(appFile)?.frontmatter ?? {}) : {};
const title = `${date} ${fm.company ?? "Interview"} ${round}`.replace(/[\\/:*?"<>|#^\[\]]/g, "-");
if (appFile) {
  if (newStage) await app.fileManager.processFrontMatter(appFile, f => { f.stage = newStage; });
  await app.vault.process(appFile, text => {
    const line = `- ${date} round: [[${title}]]${newStage ? ` (stage -> ${newStage})` : ""}`;
    const at = text.indexOf("\n## Timeline\n");
    if (at === -1) return text.trimEnd() + "\n\n## Timeline\n\n" + line + "\n";
    const next = text.indexOf("\n## ", at + 1);
    const end = next === -1 ? text.length : next;
    return text.slice(0, end).trimEnd() + "\n" + line + "\n" + text.slice(end);
  });
  await tp.file.move(`${appFile.parent.path}/${title}`);
}
-%>
---
type: round
application: "[[<% appFile ? appFile.basename : "" %>]]"
company: "<% fm.company ?? "" %>"
role: "<% fm.role ?? "" %>"
track: [<% (fm.track ?? []).join(", ") %>]
round: "<% round %>"
date: <% date %>
interviewer: ""
format: ""
outcome: pending
topics: []
tags:
  - interview-round
---

# <% fm.company ?? "" %>: <% round %>

Application: [[<% appFile ? appFile.basename : "" %>]] - Date: <% date %>

## Questions asked

Write each question as a list item tagged with its topic, for example `- Design a rate limiter #question/system-design`.
They appear in the question bank automatically.

- 

## How it went

- What went well:
- What went badly:
- Signals from the interviewer:

## What would have helped

Link the vault notes you wish you had reviewed; they feed the weak-topic list after the retrospective.

- 

## Follow-up

- [ ] Thank-you note sent
- [ ] Retrospective written (QuickAdd: Post-interview retro)
