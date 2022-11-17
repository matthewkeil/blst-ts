/* eslint-disable no-console */
import fs from "fs";
import {resolve} from "path";
// eslint-disable-next-line import/no-extraneous-dependencies
import {watch, WatchOptions} from "chokidar";

type WatchListener = (
  eventName: "add" | "addDir" | "change" | "unlink" | "unlinkDir",
  path: string,
  stats?: fs.Stats
) => void;

interface WatchProps {
  path: string;
  opts?: WatchOptions;
  waitUntilRead?: boolean;
  consoleOnNoHandler?: boolean;
  onAdd?: WatchListener;
  onAddDir?: WatchListener;
  onChange?: WatchListener;
  onUnlink?: WatchListener;
  onUnlinkDir?: WatchListener;
}

export async function watchFolder({
  path,
  opts,
  consoleOnNoHandler = false,
  onAdd,
  onAddDir,
  onChange,
  onUnlink,
  onUnlinkDir,
}: WatchProps): Promise<void> {
  const defaultHandler = (event: string, filename: string): void => {
    if (consoleOnNoHandler) {
      console.log(`default handler for ${event} ${filename}`);
    }
  };

  const watcher = watch(path, opts);
  watcher.on("all", (event, filepath) => {
    console.log(filepath);
    switch (event) {
      case "add":
        return onAdd ? onAdd(event, filepath) : defaultHandler(event, filepath);
      case "addDir":
        return onAddDir ? onAddDir(event, filepath) : defaultHandler(event, filepath);
      case "change":
        return onChange ? onChange(event, filepath) : defaultHandler(event, filepath);
      case "unlink":
        return onUnlink ? onUnlink(event, filepath) : defaultHandler(event, filepath);
      case "unlinkDir":
        return onUnlinkDir ? onUnlinkDir(event, filepath) : defaultHandler(event, filepath);
      default:
        return defaultHandler(event, filepath);
    }
  });
}

export async function watchCopyFolder(source: string, target: string, opts?: WatchOptions): Promise<void> {
  function onChange(_: string, filepath: string): Promise<void> {
    const filename = filepath.replace(`${source}/`, "");
    return fs.promises.copyFile(filepath, resolve(target, filename));
  }

  function onAddDir(_: string, filepath: string): Promise<void> {
    const filename = filepath.replace(`${source}/`, "");
    return fs.promises.mkdir(resolve(target, filename));
  }

  function onUnlink(_: string, filepath: string): Promise<void> {
    const filename = filepath.replace(`${source}/`, "");
    return fs.promises.unlink(resolve(target, filename)).catch(() => {
      console.log(`Could not delete ${filename}`);
    });
  }

  function onUnlinkDir(_: string, filepath: string): Promise<void> {
    const filename = filepath.replace(`${source}/`, "");
    return fs.promises.rmdir(resolve(target, filename), {recursive: true}).catch(() => {
      console.log(`Could not delete ${filename}`);
    });
  }

  return watchFolder({
    path: source,
    opts,
    consoleOnNoHandler: true,
    onChange,
    onAdd: onChange,
    onAddDir: onAddDir,
    onUnlink,
    onUnlinkDir,
  });
}
