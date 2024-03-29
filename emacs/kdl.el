;emacs setting for Konstantin Leivnski

;Add the following to .emacs:
;(setq load-path 
;      (cons "~/.emacs.d" load-path))
;(load-library "kdl")

;loading libraries

;; Subversion
(load-library "vc-svn")
(load-library "psvn")
(add-to-list 'vc-handled-backends 'SVN)

(tool-bar-mode 0) ; Disable toolbars and menubars for real estate.
(menu-bar-mode 0)
;; Hopfully not enabled yet.
(toggle-scroll-bar 0)

;try fonts.
(defun try-font (x)
  "Try if a certain font fits"
  (if (x-list-fonts x)
      (set-default-font x)))

(defun try-font-list (l)
  (let 
      ((to-try (car l))
       (next (cdr l)))
    (message (concat "Trying" to-try))
    (try-font (car l))
    (if next (try-font-list next))))

(try-font-list '("6x10" 
       "-raster-MS Serif-normal-r-normal-normal-8-60-96-96-p-*-iso8859-1"))


;;Interactive buffers, from http://www.emacswiki.org/emacs-en/InteractivelyDoThings#toc6.
(require 'ido)
(ido-mode t)

;;Winner mode
(when (fboundp 'winner-mode)
      (winner-mode 1))

;variables
(setq woman-use-own-frame nil); show manuals in the same frame.

(global-set-key "\M-`" 'hippie-expand) ; Come up with autocompletions; surprizingly useful.
(global-set-key "\C-ct" 'toggle-truncate-lines) ; Wrap-unwrap lines
(global-set-key "\C-cg" 'find-file-at-point) ; Go to whatever is under cursor
(global-set-key "\C-ck" 'kill-buffer) ; Alternative binding for killing buffer
(global-set-key "\C-cm" 'transient-mark-mode) ; Transient mark for local undo (COOL)
(global-set-key "\C-cf" 'follow-mode) ; Enable follow mode. 

(defun follow-mode-kludge() 
  "Enable c-mode at the same time"
  (interactive)
  (c-mode)
  (follow-mode))
(global-set-key "\C-cF" 'follow-mode-kludge) ; Enable follow mode. 

(setq truncate-partial-width-windows nil) ; Enable wrapping-inwrapping for split buffers.

;;cygwin setup

;; tweaks for cygwin; can be enabled manually, some trickery for 
;; automatic enableing provided.

(defun cygwin()
  "Make default shell to be bash"
  (interactive)
  (setq shell-file-name "bash")
  (setq shell-command-switch "-c")      ; SHOULD IT BE (setq shell-command-switch "-ic")?
  (setq explicit-shell-file-name "bash")
  (setenv "SHELL" explicit-shell-file-name)
  (setq w32-quote-process-args ?\"))

(if
    (or
     (file-exists-p "c:/bin/bash.exe")
     (file-exists-p "c:/cygwin/bin/bash.exe"))
    (progn
      (cygwin)))


;; function for customizing
(defun kdl () 
  "open kdl.el"
  (interactive)
  (find-file "~/.emacs.d/kdl.el"))

;; Dvorak.
(defun dvorak ()
  "play with dvorak layout"
  (interactive)
  (find-file "~/.emacs.d/dvorak.txt"))

;;scratch stuff
(defun kdlt () 
  "Kdl Test"
  (interactive)
  (let
      ((url (thing-at-point 'url))
       (file (thing-at-point 'filename)))
    (progn 
      (if (file-exists-p file)
	  (print "file") (print "url")))))


;;FYI
;; ~/.emacs_$SHELLNAME - startup code for shells in emacs.
