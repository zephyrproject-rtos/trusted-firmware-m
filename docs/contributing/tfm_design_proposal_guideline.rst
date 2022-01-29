#########################
Design proposal guideline
#########################

**********
Background
**********

The existing TF-M design proposal process [1]_ requires developers to upstream a
formal design document in reStructuredText format to describe their design.
That process further defines a 2-stage review process to review and approve the
design document. That process ensures that critical designs are thoroughly
reviewed by key reviewers, especially when TF-M was built up from scratch.

Currently, since more and more developers are contributing to TF-M community
with new requirements and features, the existing process sometimes becomes
heavyweight for contributors and reviewers.

  - It is an additional effort for contributors to get familiar with
    reStructuredText syntax and complete the document with reStructuredText
    coding.
  - The whole review process may take a long time. It is required that design
    document shall be approved before code review starts. However, reviews of
    design document details and implementation details sometimes can be combined
    from the point of view of reviewers.
  - The approved design document might be frequently updated during later code
    review. The changes to design documents require review and approval again.

Furthermore, it also brings some maintenance difficulties.

  - When implementation in code is changed, in theory, it is unnecessary to
    update the corresponding design document since it is a pure design proposal
    instead of a user guide.
    However, it is confusing if design document doesn't align with the
    implementation.
  - If the implementation is deprecated or the design document is out of date,
    there is no guideline yet to discuss whether or how the design document
    shall be deprecated as well or archived.

TF-M design proposal process shall be simplified to speed up contribution,
review and maintenance stages.

*************
New guideline
*************

The steps in the new design proposal guideline are lightweight and flexible to
make sure that contributors can focus more on actual code implementation and
iteration.

The guideline encourages developers to share design proposal via
TF-M mailing list [2]_ and TF-M technical forum (tech forum) [3]_.
The design details can be discussed via code reviews of actual implementations.

Typical steps are shown as the diagram below.

.. uml::

  @startuml

  title Design proposal process

  [*] --> propose : Non-trivial changes

  state "Propose general ideas" as propose {
    state "TF-M mailing list" as mail_list : Contributors send emails to mailing list\nto describe the design.
    state "TF-M tech forum" as tech_forum : Contributors present the design\nin tech forum.

    [*] --> mail_list
    [*] --> tech_forum
  }

  note bottom of propose : Optional but strongly recommended

  [*] --> upload
  note right of upload : No prerequisites
  propose --> upload

  state "Upstream changes" as upload : Contributors upstream code patch\nand integration guide to gerrit.
  state "Code review" as review : Reviewer review changes of\ncode and documents.\nChanges pass verifications.
  state "Broadcast patches" as broadcast : Contributors ask for review\nin mailing list.
  state "Approve and merge" as approve : Code owners approve changes.\nMaintainers merge patches.

  upload --> broadcast : Optional
  upload --> review

  broadcast --> review
  review --> review : Update implementation
  review --> approve
  approve --> [*]

  @enduml

Discussion in TF-M mailing list and tech forum
==============================================

It is **highly recommended** to propose and discuss designs in TF-M mailing list
or TF-M tech forum, before or while the code implementation is under review.

It is efficient and flexible to directly discuss design proposal via TF-M
mailing list and TF-M tech forum. Contributors can receive quick and broad
feedback from TF-M community.

Although it is optional to present the ideas in mailing list or tech forum, it
will help reviewers understand the design much better and expedite the code
review process.

Code review of details
======================

It is straightforward and convenient for contributors and reviewers to
deliberate over design and implementation details via code review.

Contributors can implement their design proposal and upstream the patch set to
TF-M gerrit [4]_ for code review.
For non-trivial changes or new major features, it is **strongly suggested** to
propose the design to TF-M mailing list and tech forum in advance.

Contributors don't have to wait for any approvals before upstreaming patches,
even if the changes are non-trivial.
No formal design document in advance is required anymore.

The review process is the same as the general one [5]_, with some specific
requirements:

  - Contributors can send an email to TF-M mailing list to ask for review.
  - If it requires additional reviewers besides code owners and maintainers,
    contributors shall add the specific reviewers in the review list.
  - Authors shall clearly specify the design purpose and briefly describe the
    implementation in the commit message.
  - Authors shall put essential comments and notes in code for the code changes.

Code owners and maintainers may require contributors to further verify the
implementation besides normal per-patch CI test. Contributors shall provide the
verification results as requested.

Integration guide and manual
============================

Contributors can create an integration guide or a user manual to describe how to
integrate the new features related to the design proposal.

Contributors shall update the corresponding documents if the design changes
existing implementation.

*********
Reference
*********

.. [1] :doc:`Design proposal process </docs/contributing/tfm_design_proposal_process>`

.. [2] `TF-M mailing list <https://lists.trustedfirmware.org/mailman3/lists/tf-m.lists.trustedfirmware.org/>`_

.. [3] `TF-M technical forum <https://www.trustedfirmware.org/meetings/tf-m-technical-forum/>`_

.. [4] `TF-M gerrit <https://review.trustedfirmware.org/q/project:TF-M/trusted-firmware-m>`_

.. [5] :doc:`Contributing process </docs/contributing/contributing_process>`

-------------------

*Copyright (c) 2022, Arm Limited. All rights reserved.*
